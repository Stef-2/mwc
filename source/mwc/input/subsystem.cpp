#include "mwc/input/subsystem.hpp"

import mwc_vertex_model;
import mwc_enum_bitwise_operators;
import mwc_geometry;
import mwc_enum_range;

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
      gltf_parser = fastgltf::Parser {fastgltf::Extensions::None};

      m_initialized = true;
    }
    auto input_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
    auto read_text_file(const filepath_t& a_filepath) -> string_t {
      auto input_file_stream = std::ifstream {a_filepath, std::ios::in};
      contract_assert(input_file_stream);
      auto buffer = std::stringstream {};

      buffer << input_file_stream.rdbuf();
      input_file_stream.close();

      return buffer.str();
    }
    auto read_binary_file(const filepath_t& a_filepath) -> vector_t<byte_t> {
      auto input_file_stream = std::ifstream {a_filepath, std::ios::binary bitor std::ios::in};
      contract_assert(input_file_stream);

      const auto file_size = std::filesystem::file_size(a_filepath);
      auto buffer = vector_t<byte_t>(file_size);

      input_file_stream.read(std::bit_cast<obs_ptr_t<char_t>>(buffer.data()), file_size);
      input_file_stream.close();

      return buffer;
    }
    auto read_scene_file(const filepath_t& a_filepath) -> scene_st {
      auto [data_buffer_error, data_buffer] = fastgltf::GltfDataBuffer::FromPath(a_filepath);
      contract_assert(data_buffer_error == fastgltf::Error::None);

      constexpr auto parse_options = fastgltf::Options::GenerateMeshIndices;
      const auto& [parse_error, asset] = input_subsystem_st::gltf_parser.loadGltfBinary(data_buffer, a_filepath, parse_options);
      contract_assert(parse_error == fastgltf::Error::None);
      contract_assert(asset.scenes.size() == 1);

      auto scene = scene_st {};
      scene.m_meshes.reserve(asset.meshes.size());

      for (auto i = 0uz; i < asset.meshes.size(); ++i) {
        const auto& mesh = asset.meshes[i];

        scene.m_meshes.emplace_back();

        for (const auto& primitive : mesh.primitives) {
          auto accessor_offset_pairs = std::vector<std::pair<const fastgltf::Accessor*, std::size_t>> {};
          accessor_offset_pairs.reserve(utility::enum_range<geometry::vertex_component_bit_mask_et>().size());

          // determine which vector components are present
          for (const auto component : utility::bit_mask_enum_range<geometry::vertex_component_bit_mask_et>()) {
            const auto vertex_component = geometry::vertex_component_bit_mask_et {component};
            const auto component_string = gltf_vertex_component_string(vertex_component);
            const auto attribute = primitive.findAttribute(component_string);

            if (attribute == primitive.attributes.cend())
              continue;

            accessor_offset_pairs.emplace_back(&asset.accessors[attribute->accessorIndex],
                                               scene.m_meshes.back().m_vertex_model_size);
            scene.m_meshes.back().m_vertex_model_size += geometry::vertex_component_size(vertex_component);
            scene.m_meshes.back().m_vertex_component_bit_mask |= component;
          }
          scene.m_meshes.back().m_vertex_storage.resize(
            scene.m_meshes.back().m_vertex_model_size * accessor_offset_pairs.front().first->count);
          scene.m_meshes.back().m_index_storage.resize(asset.accessors[primitive.indicesAccessor.value()].count);

          using namespace geometry;
          using vertex_component_tuple_t =
            tuple_t<vertex_position_st, vertex_normal_st, vertex_tangent_st, vertex_uv_st, vertex_color_st, vertex_joints_st, vertex_weights_st>;
          // write vertex component data
          auto lambda = [&asset, &scene, &accessor_offset_pairs]<size_t tp_index, size_t... tp_indices>(
                          this auto&& a_this,
                          std::index_sequence<tp_index, tp_indices...>) {
            if (scene.m_meshes.back().m_vertex_component_bit_mask bitand utility::set_bit<tp_index + 1>())
              fastgltf::iterateAccessorWithIndex<typename std::tuple_element_t<tp_index, vertex_component_tuple_t>::array_t>(
                asset,
                *accessor_offset_pairs[tp_index].first,
                [&](const auto& a_component, const size_t a_index) {
                  std::memcpy(scene.m_meshes.back().m_vertex_storage.data() +
                                (a_index * scene.m_meshes.back().m_vertex_model_size + accessor_offset_pairs[tp_index].second),
                              &a_component, sizeof(a_component));
                });
            if constexpr (sizeof...(tp_indices) > 0)
              a_this(std::index_sequence<tp_indices...> {});
          };
          lambda(std::make_index_sequence<std::tuple_size_v<vertex_component_tuple_t>> {});

          // write vertex index data
          fastgltf::copyFromAccessor<decltype(scene)::mesh_storage_t::value_type::index_storage_t::value_type>(
            asset,
            asset.accessors[primitive.indicesAccessor.value()],
            scene.m_meshes.back().m_index_storage.data());
        }
        // iterate scene nodes
        fastgltf::iterateSceneNodes(
          asset,
          0,
          fastgltf::math::fmat4x4 {1.0},
          [&scene, &i](const fastgltf::Node& a_node, const fastgltf::math::fmat4x4& a_transform_matrix) {
            static_assert(sizeof(decltype(a_transform_matrix)) == sizeof(geometry::transformation_t<>));

            if (a_node.meshIndex.value() == i)
              std::memcpy(scene.m_meshes.back().m_transformation.data(),
                          a_transform_matrix.data(),
                          sizeof(geometry::transformation_t<>));
          });
      }
      return scene;
    }

  }
}