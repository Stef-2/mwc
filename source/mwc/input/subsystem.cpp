#include "mwc/input/subsystem.hpp"

namespace mwc {
  namespace input {
    auto input_subsystem_st::initialize() -> void {
      keyboard_st::key_map.reserve(std::to_underlying(vkfw::Key::eLAST));
      mouse_st::key_map.reserve(std::to_underlying(vkfw::MouseButton::eLAST));
      filesystem_st::gltf_parser = fastgltf::Parser {fastgltf::Extensions::None};

      m_initialized = true;
    }
    auto input_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
    auto input_subsystem_st::poll_hardware_events() -> decltype(vkfw::pollEvents()) {
      return vkfw::pollEvents();
    }
    auto read_text_file(const file_path_t& a_filepath) -> string_t {
      auto input_file_stream = std::ifstream {a_filepath, std::ios::in};
      contract_assert(input_file_stream);
      auto buffer = std::stringstream {};

      buffer << input_file_stream.rdbuf();
      input_file_stream.close();

      return buffer.str();
    }
    auto read_binary_file(const file_path_t& a_filepath) -> vector_t<byte_t> {
      auto input_file_stream = std::ifstream {a_filepath, std::ios::binary bitor std::ios::in};
      contract_assert(input_file_stream);

      const auto file_size = std::filesystem::file_size(a_filepath);
      auto buffer = vector_t<byte_t>(file_size);

      input_file_stream.read(std::bit_cast<obs_ptr_t<char_t>>(buffer.data()), file_size);
      input_file_stream.close();

      return buffer;
    }
    auto read_scene_file(const file_path_t& a_filepath,
                         const input_subsystem_st::filesystem_st::scene_read_configuration_st& a_configuration)
      -> std::optional<input_subsystem_st::filesystem_st::processed_resources_st> {
      // scene mesh and image data should either be preserved in host memory or propagated to device memory, or both
      if (not a_configuration.m_mesh_processing.m_preserve_in_host_memory
          and not a_configuration.m_mesh_processing.m_propagate_to_device_memory)
        warning(std::format("scene read from {0} is not configured for mesh processing", a_filepath.c_str()));
      if (not a_configuration.m_image_processing.m_preserve_in_host_memory
          and not a_configuration.m_image_processing.m_propagate_to_device_memory)
        warning(std::format("scene read from {0} is not configured for image processing", a_filepath.c_str()));

      auto [data_buffer_error, data_buffer] = fastgltf::GltfDataBuffer::FromPath(a_filepath);
      contract_assert(data_buffer_error == fastgltf::Error::None);

      constexpr auto parse_options = fastgltf::Options::GenerateMeshIndices;
      auto&& [parse_error, asset]
        = input_subsystem_st::filesystem_st::gltf_parser.loadGltfBinary(data_buffer, a_filepath, parse_options);
      contract_assert(parse_error == fastgltf::Error::None);
      // assert one scene per file
      contract_assert(asset.scenes.size() == 1);

      const auto& gltf_scene = asset.scenes.front();
      auto native_scene = scene_st {a_filepath, gltf_scene.name};
      native_scene.m_nodes = decltype(scene_st::m_nodes)::configuration_st {asset.nodes.size()},
      native_scene.m_meshes.reserve(asset.meshes.size());
      native_scene.m_images.reserve(asset.images.size());

      // iterate scene meshes
      for (auto i = 0uz; i < asset.meshes.size(); ++i) {
        const auto& mesh = asset.meshes[i];

        native_scene.m_meshes.emplace_back();
        for (const auto& primitive : mesh.primitives) {
          auto accessor_offset_pairs = std::vector<std::pair<const fastgltf::Accessor*, size_t>> {};
          accessor_offset_pairs.reserve(utility::enum_range<geometry::vertex_component_bit_mask_et>().size());

          // determine which components the mash has
          for (const auto component : utility::bit_mask_enum_range<geometry::vertex_component_bit_mask_et>()) {
            const auto native_vertex_component = geometry::vertex_component_bit_mask_et {component};
            const auto component_string = gltf_vertex_component_string(native_vertex_component);
            const auto attribute = primitive.findAttribute(component_string);

            if (attribute == primitive.attributes.cend())
              continue;

            accessor_offset_pairs.emplace_back(&asset.accessors[attribute->accessorIndex],
                                               native_scene.m_meshes.back().m_vertex_model_size);
            native_scene.m_meshes.back().m_vertex_model_size += geometry::vertex_component_size(native_vertex_component);
            native_scene.m_meshes.back().m_vertex_component_bit_mask |= component;
          }
          const auto vertex_count = accessor_offset_pairs.front().first->count;
          if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
            native_scene.m_meshes.back().m_vertex_storage.resize(native_scene.m_meshes.back().m_vertex_model_size * vertex_count);
            native_scene.m_meshes.back().m_index_storage.resize(asset.accessors[primitive.indicesAccessor.value()].count);
          }
          native_scene.m_meshes.back().m_aabb = geometry::aabb_st {.m_min = {0.0, 0.0, 0.0}, .m_max = {0.0, 0.0, 0.0}};

          using namespace geometry;
          using vertex_component_tuple_t
            = tuple_t<vertex_position_st, vertex_normal_st, vertex_tangent_st, vertex_uv_st, vertex_color_st, vertex_joints_st, vertex_weights_st>;
          // write vertex component data
          static_for_loop<0, std::tuple_size_v<vertex_component_tuple_t>>(
            [&a_configuration, &asset, &native_scene, &accessor_offset_pairs]<size_t tp_index> {
              if (native_scene.m_meshes.back().m_vertex_component_bit_mask bitand utility::set_bit<tp_index + 1>())
                fastgltf::iterateAccessorWithIndex<typename std::tuple_element_t<tp_index, vertex_component_tuple_t>::array_t>(
                  asset,
                  *accessor_offset_pairs[tp_index].first,
                  [&](const auto& a_component, const size_t a_index) {
                    if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
                      std::memcpy(native_scene.m_meshes.back().m_vertex_storage.data()
                                    + (a_index * native_scene.m_meshes.back().m_vertex_model_size
                                       + accessor_offset_pairs[tp_index].second),
                                  &a_component, sizeof(a_component));
                    }
                    // generate bounding boxes
                    if constexpr (std::is_same_v<std::tuple_element_t<tp_index, vertex_component_tuple_t>, vertex_position_st>) {
                      native_scene.m_meshes.back().m_aabb.m_min
                        = native_scene.m_meshes.back().m_aabb.m_min.cwiseMin(position_t<> {a_component.data()});
                      native_scene.m_meshes.back().m_aabb.m_max
                        = native_scene.m_meshes.back().m_aabb.m_max.cwiseMax(position_t<> {a_component.data()});
                    }
                  });
            });
        }
        // write vertex index data
        if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
          fastgltf::copyFromAccessor<decltype(native_scene)::mesh_storage_t::value_type::index_storage_t::value_type>(
            asset,
            asset.accessors[mesh.primitives.front().indicesAccessor.value()],
            native_scene.m_meshes.back().m_index_storage.data());
        }
      }
      // iterate scene nodes
      const auto root_node_count = gltf_scene.nodeIndices.size();
      if (root_node_count > 1) {
        warning(std::format("scene loaded from {0} has {1} root nodes", a_filepath.c_str(), root_node_count));
      }

      const auto iterate_node_hierarchy
        = [&asset, &native_scene](this auto&& a_this, const decltype(gltf_scene.nodeIndices)::value_type a_parent_index) -> void {
        for (const auto& child_index : asset.nodes[a_parent_index].children) {
          const auto& child_node = asset.nodes[child_index];
          const auto transformation_matrix = fastgltf::getTransformMatrix(child_node);
          const auto mesh_index = child_node.meshIndex ? child_node.meshIndex.value() : scene_st::node_data_st::null_mesh_index;
          native_scene.m_nodes.insert_node(
            scene_st::node_data_st {.m_transformation = *std::bit_cast<geometry::transformation_t<>*>(&transformation_matrix),
                                    .m_mesh_index = static_cast<scene_st::node_data_st::index_t>(mesh_index)},
            a_parent_index);

          // call recursively
          a_this(child_index);
        }
      };

      for (auto i = 0uz; i < root_node_count; ++i) {
        const auto& root_node = asset.nodes[gltf_scene.nodeIndices[i]];
        const auto transformation_matrix = fastgltf::getTransformMatrix(root_node);
        const auto mesh_index = root_node.meshIndex ? root_node.meshIndex.value() : scene_st::node_data_st::null_mesh_index;
        native_scene.m_nodes.insert_node(
          scene_st::node_data_st {.m_transformation = *std::bit_cast<geometry::transformation_t<>*>(&transformation_matrix),
                                  .m_mesh_index = static_cast<scene_st::node_data_st::index_t>(mesh_index)},
          gltf_scene.nodeIndices[i]);
        iterate_node_hierarchy(gltf_scene.nodeIndices[i]);
      }
      contract_assert(native_scene.m_nodes.node_count() == asset.nodes.size());

      // iterate scene images
      for (auto& gltf_image : asset.images) {
        constexpr auto requested_channel_count = 4;

        contract_assert(std::holds_alternative<fastgltf::sources::BufferView>(gltf_image.data));
        auto& buffer_view = asset.bufferViews[std::get<fastgltf::sources::BufferView>(gltf_image.data).bufferViewIndex];

        auto& buffer = asset.buffers[buffer_view.bufferIndex].data;
        contract_assert(std::holds_alternative<fastgltf::sources::Array>(buffer));
        auto& byte_array = std::get<fastgltf::sources::Array>(buffer).bytes;

        auto data = span_t<byte_t> {byte_array.begin() + buffer_view.byteOffset, buffer_view.byteLength};

        auto image = dynamic_image_st {resource_st {a_filepath, gltf_image.name}};
        image.m_data.resize(data.size_bytes());

        const auto image_bytes = stbi_load_from_memory(std::bit_cast<const stbi_uc*>(data.data()),
                                                       data.size(),
                                                       std::bit_cast<int*>(&image.m_width),
                                                       std::bit_cast<int*>(&image.m_height),
                                                       std::bit_cast<int*>(&image.m_channel_count),
                                                       requested_channel_count);

        contract_assert(image_bytes);

        if (a_configuration.m_image_processing.m_preserve_in_host_memory) {
          std::memcpy(image.m_data.data(), image_bytes, image.m_data.size());
        }
        information(std::format("loaded image: {0}" SUB "width: {1} height: {2} channel count: {3}",
                                image.m_name,
                                image.m_width,
                                image.m_height,
                                image.m_channel_count));
        stbi_image_free(image_bytes);

        native_scene.m_images.emplace_back(std::move(image));
      }

      input_subsystem_st::filesystem_st::scene_registry.emplace_back(native_scene);

      return {};
    }
  }
}