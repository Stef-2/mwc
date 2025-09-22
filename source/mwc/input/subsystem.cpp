#include "mwc/input/subsystem.hpp"

import mwc_vertex_model;
import mwc_enum_bitwise_operators;
import mwc_metaprogramming_utility;
import mwc_geometry;
import mwc_enum_range;

import fastgltf;

// vertex component specializations
template <>
struct fastgltf::ElementTraits<mwc::geometry::vertex_uv_st::array_t>
: fastgltf::ElementTraitsBase<mwc::geometry::vertex_uv_st::array_t, AccessorType::Vec2, mwc::geometry::vertex_uv_st::array_t::value_type> {
};
template <>
struct fastgltf::ElementTraits<mwc::geometry::vertex_position_st::array_t>
: fastgltf::ElementTraitsBase<mwc::geometry::vertex_position_st::array_t,
                              AccessorType::Vec3,
                              mwc::geometry::vertex_position_st::array_t::value_type> {};
template <>
struct fastgltf::ElementTraits<mwc::geometry::vertex_tangent_st::array_t>
: fastgltf::ElementTraitsBase<mwc::geometry::vertex_tangent_st::array_t,
                              AccessorType::Vec4,
                              mwc::geometry::vertex_tangent_st::array_t::value_type> {};
template <>
struct fastgltf::ElementTraits<mwc::geometry::vertex_joints_st::array_t>
: fastgltf::ElementTraitsBase<mwc::geometry::vertex_joints_st::array_t,
                              AccessorType::Vec4,
                              mwc::geometry::vertex_joints_st::array_t::value_type> {};
namespace mwc {
  namespace input {
    static inline auto gltf_parser = fastgltf::Parser {};

    constexpr auto gltf_vertex_component_string(const geometry::vertex_component_bit_mask_et a_component) {
      switch (a_component) {
        using enum geometry::vertex_component_bit_mask_et;
        case e_position : return "POSITION";
        case e_normal : return "NORMAL";
        case e_tangent : return "TANGENT";
        case e_uv : return "TEXCOORD_0";
        case e_color : return "COLOR_0";
        case e_joints : return "JOINTS_0";
        case e_weights : return "WEIGHTS_0";
        default : contract_assert(false); std::unreachable();
      }
    }
    auto input_subsystem_st::initialize() -> void {
      keyboard_st::key_map.reserve(std::to_underlying(vkfw::Key::eLAST));
      mouse_st::key_map.reserve(std::to_underlying(vkfw::MouseButton::eLAST));
      gltf_parser = fastgltf::Parser {fastgltf::Extensions::None};

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
    auto read_scene_file(const file_path_t& a_filepath) -> scene_st {
      auto [data_buffer_error, data_buffer] = fastgltf::GltfDataBuffer::FromPath(a_filepath);
      contract_assert(data_buffer_error == fastgltf::Error::None);

      constexpr auto parse_options = fastgltf::Options::GenerateMeshIndices;
      const auto& [parse_error, asset] = gltf_parser.loadGltfBinary(data_buffer, a_filepath, parse_options);
      contract_assert(parse_error == fastgltf::Error::None);
      // assert one scene per file
      contract_assert(asset.scenes.size() == 1);

      const auto& gltf_scene = asset.scenes.front();
      auto native_scene = scene_st {
        .m_meshes = {},
        .m_nodes = decltype(scene_st::m_nodes)::configuration_st {asset.nodes.size()},
      };
      native_scene.m_meshes.reserve(asset.meshes.size());

      // iterate scene meshes
      for (auto i = 0uz; i < asset.meshes.size(); ++i) {
        const auto& mesh = asset.meshes[i];

        native_scene.m_meshes.emplace_back();
        native_scene.m_meshes.back().m_name = mesh.name;

        for (const auto& primitive : mesh.primitives) {
          auto accessor_offset_pairs = std::vector<std::pair<const fastgltf::Accessor*, std::size_t>> {};
          accessor_offset_pairs.reserve(utility::enum_range<geometry::vertex_component_bit_mask_et>().size());

          // determine which components the mash has
          for (const auto component : utility::bit_mask_enum_range<geometry::vertex_component_bit_mask_et>()) {
            const auto vertex_component = geometry::vertex_component_bit_mask_et {component};
            const auto component_string = gltf_vertex_component_string(vertex_component);
            const auto attribute = primitive.findAttribute(component_string);

            if (attribute == primitive.attributes.cend())
              continue;

            accessor_offset_pairs.emplace_back(&asset.accessors[attribute->accessorIndex],
                                               native_scene.m_meshes.back().m_vertex_model_size);
            native_scene.m_meshes.back().m_vertex_model_size += geometry::vertex_component_size(vertex_component);
            native_scene.m_meshes.back().m_vertex_component_bit_mask |= component;
          }
          native_scene.m_meshes.back().m_vertex_storage.resize(
            native_scene.m_meshes.back().m_vertex_model_size * accessor_offset_pairs.front().first->count);
          native_scene.m_meshes.back().m_index_storage.resize(asset.accessors[primitive.indicesAccessor.value()].count);

          using namespace geometry;
          using vertex_component_tuple_t
            = tuple_t<vertex_position_st, vertex_normal_st, vertex_tangent_st, vertex_uv_st, vertex_color_st, vertex_joints_st, vertex_weights_st>;
          // write vertex component data
          static_for_loop<0, std::tuple_size_v<vertex_component_tuple_t>>(
            [&asset, &native_scene, &accessor_offset_pairs]<size_t tp_index> {
              if (native_scene.m_meshes.back().m_vertex_component_bit_mask bitand utility::set_bit<tp_index + 1>())
                fastgltf::iterateAccessorWithIndex<typename std::tuple_element_t<tp_index, vertex_component_tuple_t>::array_t>(
                  asset,
                  *accessor_offset_pairs[tp_index].first,
                  [&](const auto& a_component, const size_t a_index) {
                    std::memcpy(
                      native_scene.m_meshes.back().m_vertex_storage.data()
                        + (a_index * native_scene.m_meshes.back().m_vertex_model_size + accessor_offset_pairs[tp_index].second),
                      &a_component, sizeof(a_component));
                  });
            });
          // write vertex index data
          fastgltf::copyFromAccessor<decltype(native_scene)::mesh_storage_t::value_type::index_storage_t::value_type>(
            asset,
            asset.accessors[primitive.indicesAccessor.value()],
            native_scene.m_meshes.back().m_index_storage.data());
        }
      }
      // iterate scene nodes
      const auto root_node_count = gltf_scene.nodeIndices.size();
      if (root_node_count > 1)
        mwc::warning(std::format("scene loaded from {0} has {1} root nodes", a_filepath.c_str(), root_node_count));

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

      return native_scene;
    }
  }
}