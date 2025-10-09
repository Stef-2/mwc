#include "mwc/input/subsystem.hpp"

#include <stb/stb_image.h>

import mwc_breakpoint;

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
      -> optional_t<input_subsystem_st::filesystem_st::resource_device_memory_st> {
      // sanity checks
      // scene mesh and image data should either be preserved in host memory or propagated to device memory, or both
      if (not a_configuration.m_mesh_processing.m_preserve_in_host_memory
          and not a_configuration.m_mesh_processing.m_propagate_to_device_memory)
        warning(std::format("scene read from {0} is not configured for mesh processing", a_filepath.c_str()));
      if (not a_configuration.m_image_processing.m_preserve_in_host_memory
          and not a_configuration.m_image_processing.m_propagate_to_device_memory)
        warning(std::format("scene read from {0} is not configured for image processing", a_filepath.c_str()));

      const auto device_memory_processing_requested = a_configuration.m_mesh_processing.m_propagate_to_device_memory
                                                   or a_configuration.m_image_processing.m_propagate_to_device_memory;
      if (device_memory_processing_requested and not a_configuration.m_device_buffer)
        error(
          std::format("scene read from {0} is configured for device memory processing, but no virtual allocator was provided",
                      a_filepath.c_str()));
      if (a_configuration.m_device_buffer and not device_memory_processing_requested)
        warning(std::format(
          "scene read from {0} is not configured for device memory processing, but a virtual allocator was provided",
          a_filepath.c_str()));

      auto [data_buffer_error, data_buffer] = fastgltf::GltfDataBuffer::FromPath(a_filepath);
      contract_assert(data_buffer_error == fastgltf::Error::None);

      constexpr auto parse_options = fastgltf::Options::GenerateMeshIndices;
      auto&& [parse_error, asset]
        = input_subsystem_st::filesystem_st::gltf_parser.loadGltfBinary(data_buffer, a_filepath, parse_options);
      contract_assert(parse_error == fastgltf::Error::None);
      // assert one scene per file
      contract_assert(asset.scenes.size() == 1);

      const auto& gltf_scene = asset.scenes.front();
      auto native_scene = scene_st {};
      native_scene.m_nodes = decltype(scene_st::m_nodes)::configuration_st {asset.nodes.size()},
      native_scene.m_meshes.reserve(asset.meshes.size());
      native_scene.m_images.reserve(asset.images.size());

      auto resources_device_memory = optional_t<input_subsystem_st::filesystem_st::resource_device_memory_st> {};
      if (device_memory_processing_requested)
        resources_device_memory = input_subsystem_st::filesystem_st::resource_device_memory_st {};

      // initial mesh processing
      constexpr auto index_byte_size
        = sizeof(typename decltype(native_scene)::mesh_storage_t::value_type::index_storage_t::value_type);
      auto mesh_accessor_offset_pairs = vector_t<pair_t<const fastgltf::Accessor*, size_t>> {};
      mesh_accessor_offset_pairs.reserve(
        utility::enum_range<geometry::vertex_component_bit_mask_et>().size() * asset.meshes.size());

      auto mesh_data_byte_count = size_t {0};
      for (auto i = 0uz; i < asset.meshes.size(); ++i) {
        const auto& mesh = asset.meshes[i];

        native_scene.m_meshes.emplace_back(resource_st {a_filepath, mesh.name});
        auto& native_mesh = native_scene.m_meshes.back();
        for (const auto& primitive : mesh.primitives) {
          // determine which components the mash has
          for (const auto component : utility::bit_mask_enum_range<geometry::vertex_component_bit_mask_et>()) {
            const auto native_vertex_component = geometry::vertex_component_bit_mask_et {component};
            const auto component_string = gltf_vertex_component_string(native_vertex_component);
            const auto attribute = primitive.findAttribute(component_string);

            if (attribute == primitive.attributes.cend())
              continue;

            mesh_accessor_offset_pairs.emplace_back(&asset.accessors[attribute->accessorIndex], native_mesh.m_vertex_model_size);
            native_mesh.m_vertex_model_size += geometry::vertex_component_size(native_vertex_component);
            native_mesh.m_vertex_component_bit_mask |= component;
          }
        }
        native_mesh.m_vertex_count = mesh_accessor_offset_pairs.back().first->count;
        native_mesh.m_index_count = asset.accessors[mesh.primitives.front().indicesAccessor.value()].count;

        if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
          native_mesh.m_vertex_storage.resize(native_mesh.m_vertex_model_size * native_mesh.m_vertex_count);
          native_mesh.m_index_storage.resize(native_mesh.m_index_count);

          static_assert(sizeof(typename std::remove_cvref_t<decltype(native_mesh)>::vertex_storage_t::value_type)
                        == sizeof(byte_t));
          mesh_data_byte_count += native_mesh.m_vertex_storage.size() + native_mesh.m_index_storage.size() * index_byte_size;
        }
        native_mesh.m_aabb = geometry::aabb_st {.m_min = {0.0, 0.0, 0.0}, .m_max = {0.0, 0.0, 0.0}};
      }
      // generate a virtual suballocation for transfer to device memory
      if (a_configuration.m_mesh_processing.m_propagate_to_device_memory) {
        resources_device_memory.value().m_memory_mapped_device_mesh_data
          = a_configuration.m_device_buffer->request_suballocation<byte_t>(mesh_data_byte_count);
      }
      // transfer mesh data
      for (auto i = 0uz, accessor_index = 0uz, device_memory_offset = 0uz; i < asset.meshes.size(); ++i) {
        auto& native_mesh = native_scene.m_meshes[i];
        using namespace geometry;
        using vertex_component_tuple_t
          = tuple_t<vertex_position_st, vertex_normal_st, vertex_tangent_st, vertex_uv_st, vertex_color_st, vertex_joints_st, vertex_weights_st>;
        // write vertex component data
        static_for_loop<0, std::tuple_size_v<vertex_component_tuple_t>>([&a_configuration,
                                                                         &asset,
                                                                         &resources_device_memory,
                                                                         &mesh_accessor_offset_pairs,
                                                                         &native_mesh,
                                                                         &accessor_index,
                                                                         &device_memory_offset]<size_t tp_index> {
          if (native_mesh.m_vertex_component_bit_mask bitand utility::set_bit<tp_index + 1>()) {
            fastgltf::iterateAccessorWithIndex<typename std::tuple_element_t<tp_index, vertex_component_tuple_t>::array_t>(
              asset,
              *mesh_accessor_offset_pairs[accessor_index].first,
              [&](const auto& a_component, const size_t a_index) {
                const auto data_offset
                  = a_index * native_mesh.m_vertex_model_size + mesh_accessor_offset_pairs[accessor_index].second;
                // transfer to host memory
                if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
                  std::memcpy(native_mesh.m_vertex_storage.data() + data_offset, &a_component, sizeof(a_component));
                }
                // transfer to device memory
                if (a_configuration.m_mesh_processing.m_propagate_to_device_memory) {

                  std::memcpy(resources_device_memory.value().m_memory_mapped_device_mesh_data.data() + data_offset
                                + device_memory_offset,
                              &a_component,
                              sizeof(a_component));
                }
                // generate bounding boxes
                if constexpr (std::is_same_v<std::tuple_element_t<tp_index, vertex_component_tuple_t>, vertex_position_st>) {
                  native_mesh.m_aabb.m_min = native_mesh.m_aabb.m_min.cwiseMin(position_t<> {a_component.data()});
                  native_mesh.m_aabb.m_max = native_mesh.m_aabb.m_max.cwiseMax(position_t<> {a_component.data()});
                }
              });
            ++accessor_index;
          }
        });
        // write vertex index data
        if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
          fastgltf::copyFromAccessor<decltype(native_scene)::mesh_storage_t::value_type::index_storage_t::value_type>(
            asset,
            asset.accessors[asset.meshes[i].primitives.front().indicesAccessor.value()],
            native_mesh.m_index_storage.data());
        }
        if (a_configuration.m_mesh_processing.m_propagate_to_device_memory) {
          device_memory_offset += native_mesh.m_vertex_count * native_mesh.m_vertex_model_size;
          const auto destination = resources_device_memory.value().m_memory_mapped_device_mesh_data.data() + device_memory_offset;
          fastgltf::copyFromAccessor<decltype(native_scene)::mesh_storage_t::value_type::index_storage_t::value_type>(
            asset,
            asset.accessors[asset.meshes[i].primitives.front().indicesAccessor.value()],
            destination);
          device_memory_offset += native_mesh.m_index_count * index_byte_size;
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

      auto raw_image_data = vector_t<span_t<byte_t>> {};
      auto raw_image_data_byte_count = size_t {0};
      raw_image_data.reserve(asset.images.size());

      // initial image processing
      for (auto& gltf_image : asset.images) {
        constexpr auto requested_channel_count = 4;

        contract_assert(std::holds_alternative<fastgltf::sources::BufferView>(gltf_image.data));
        auto& gltf_buffer_view = asset.bufferViews[std::get<fastgltf::sources::BufferView>(gltf_image.data).bufferViewIndex];

        auto& gltf_buffer = asset.buffers[gltf_buffer_view.bufferIndex].data;
        contract_assert(std::holds_alternative<fastgltf::sources::Array>(gltf_buffer));
        auto& byte_array = std::get<fastgltf::sources::Array>(gltf_buffer).bytes;

        auto data = span_t<byte_t> {byte_array.begin() + gltf_buffer_view.byteOffset, gltf_buffer_view.byteLength};

        auto image = dynamic_image_st {resource_st {a_filepath, gltf_image.name}};

        const auto image_bytes
          = std::bit_cast<obs_ptr_t<byte_t>>(stbi_load_from_memory(std::bit_cast<const stbi_uc*>(data.data()),
                                                                   data.size(),
                                                                   std::bit_cast<int*>(&image.m_width),
                                                                   std::bit_cast<int*>(&image.m_height),
                                                                   std::bit_cast<int*>(&image.m_channel_count),
                                                                   requested_channel_count));

        contract_assert(image_bytes);
        raw_image_data.emplace_back(image_bytes, data.size_bytes());
        raw_image_data_byte_count += data.size_bytes();

        //stbi_image_free(image_bytes);

        native_scene.m_images.emplace_back(resource_st {a_filepath, gltf_image.name});
      }
      // generate a virtual suballocation for transfer to device memory
      if (a_configuration.m_image_processing.m_propagate_to_device_memory) {
        resources_device_memory.value().m_memory_mapped_device_image_data
          = a_configuration.m_device_buffer->request_suballocation<byte_t>(raw_image_data_byte_count);
      }
      // transfer image data
      for (auto i = 0uz; i < native_scene.m_images.size(); ++i) {
        // transfer to host memory
        if (a_configuration.m_image_processing.m_preserve_in_host_memory) {
          native_scene.m_images[i].m_data.resize(raw_image_data[i].size_bytes());
          std::memcpy(native_scene.m_images[i].m_data.data(), raw_image_data[i].data(), raw_image_data[i].size_bytes());
        }
        if (a_configuration.m_image_processing.m_propagate_to_device_memory) {
        }
        stbi_image_free(raw_image_data[i].data());
      }

      input_subsystem_st::filesystem_st::scene_registry.emplace_back(native_scene);

      return {};
    }
  }
}