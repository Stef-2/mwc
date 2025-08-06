#include "mwc/input/subsystem.hpp"

import mwc_vertex_model;
import mwc_enum_bitwise_operators;
import mwc_enum_range;

template <>
struct fastgltf::ElementTraits<mwc::geometry::vertex_position_st::vertex_position_t>
: fastgltf::ElementTraitsBase<mwc::geometry::vertex_position_st::vertex_position_t, AccessorType::Vec3,
                              mwc::geometry::vertex_position_st::vertex_position_t::Scalar> {};
template <>
struct fastgltf::ElementTraits<mwc::array_t<mwc::geometry::vertex_position_st::vertex_position_t::Scalar, 3>>
: fastgltf::ElementTraitsBase<mwc::array_t<mwc::geometry::vertex_position_st::vertex_position_t::Scalar, 3>, AccessorType::Vec3,
                              mwc::geometry::vertex_position_st::vertex_position_t::Scalar> {};
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
      const auto [parse_error, asset] = input_subsystem_st::gltf_parser.loadGltf(data_buffer, a_filepath, parse_options);
      contract_assert(parse_error == fastgltf::Error::None);

      auto scene = scene_st {};
      scene.m_meshes.reserve(asset.meshes.size());

      for (const auto& mesh : asset.meshes) {
        scene.m_meshes.emplace_back();

        for (const auto& primitive : mesh.primitives) {
          auto accessors =
            vector_t<obs_ptr_t<const fastgltf::Accessor>>(utility::enum_range<geometry::vertex_component_bit_mask_et>().size());
          auto vertex_model_size = size_t {0};

          for (const auto component : utility::enum_range<geometry::vertex_component_bit_mask_et>()) {
            const auto vertex_component = geometry::vertex_component_bit_mask_et {component};
            const auto component_string = gltf_vertex_component_string(vertex_component);
            const auto attribute = primitive.findAttribute(component_string);
            if (not attribute)
              continue;

            accessors.emplace_back(&asset.accessors[attribute->accessorIndex]);
            vertex_model_size += geometry::vertex_component_size(vertex_component);
            scene.m_meshes.back().m_vertex_component_bit_mask bitor component;
          }

          const auto position_attribute = primitive.findAttribute("POSITION");
          const auto normal_attribute = primitive.findAttribute("NORMAL");
          const auto tangent_attribute = primitive.findAttribute("TANGENT");
          const auto uv_attribute = primitive.findAttribute("TEXCOORD_0");

          const auto& position_accessor = asset.accessors[position_attribute->accessorIndex];
          const auto& normal_accessor = asset.accessors[normal_attribute->accessorIndex];
          const auto& tangent_accessor = asset.accessors[tangent_attribute->accessorIndex];
          const auto& uv_accessor = asset.accessors[uv_attribute->accessorIndex];
          const auto& index_accessor = asset.accessors[primitive.indicesAccessor.value()];

          std::vector<float32_t> tvec(uv_accessor.count * 2);

          fastgltf::copyFromAccessor<fastgltf::math::f32vec2, sizeof(fastgltf::math::f32vec2) * 2>(asset, uv_accessor,
                                                                                                   tvec.data());

          fastgltf::iterateAccessorWithIndex<array_t<geometry::vertex_position_st::vertex_position_t::Scalar, 3>>(
            asset, position_accessor, [&](auto a_position, size_t a_index) {
              scene.m_meshes.back().m_vertex_storage.append_range(std::bit_cast<array_t<byte_t, sizeof(a_position)>>(a_position));
            });
        }
      }

      return scene;
    }
  }
}