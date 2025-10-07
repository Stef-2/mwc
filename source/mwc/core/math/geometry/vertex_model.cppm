module;

#include "mwc/core/contract/definition.hpp"

export module mwc_vertex_model;

import mwc_definition;
import mwc_math_definition;
import mwc_metaprogramming_utility;
import mwc_set_bit;
import mwc_concept;
import mwc_geometry;

import fastgltf;

import std;

export namespace mwc {
  namespace geometry {
    enum class vertex_component_bit_mask_et {
      e_position = utility::set_bit<1>(),
      e_normal = utility::set_bit<2>(),
      e_tangent = utility::set_bit<3>(),
      e_uv = utility::set_bit<4>(),
      e_color = utility::set_bit<5>(),
      e_joints = utility::set_bit<6>(),
      e_weights = utility::set_bit<7>(),
      end
    };

    // vertex components
    struct vertex_position_st {
      using storage_t = position_t<>;
      using array_t = array_t<storage_t::Scalar, storage_t::RowsAtCompileTime>;

      storage_t m_position;
    };
    struct vertex_normal_st {
      using storage_t = normal_t<>;
      using array_t = array_t<storage_t::Scalar, storage_t::RowsAtCompileTime>;

      storage_t m_normal;
    };
    struct vertex_tangent_st {
      using storage_t = math::vector_t<vertex_normal_st::storage_t::Scalar, 4>;
      using array_t = array_t<storage_t::Scalar, storage_t::RowsAtCompileTime>;

      storage_t m_tangent;
    };
    struct vertex_uv_st {
      using storage_t = math::vector_t<float32_t, 2>;
      using array_t = array_t<storage_t::Scalar, storage_t::RowsAtCompileTime>;

      storage_t m_uv;
    };
    struct vertex_color_st {
      using storage_t = math::vector_t<float32_t, 3>;
      using array_t = array_t<storage_t::Scalar, storage_t::RowsAtCompileTime>;

      storage_t m_color;
    };
    struct vertex_joints_st {
      using storage_t = math::vector_t<uint16_t, 4>;
      using array_t = array_t<storage_t::Scalar, storage_t::RowsAtCompileTime>;

      storage_t m_joints;
    };
    struct vertex_weights_st {
      using storage_t = math::vector_t<float32_t, 4>;
      using array_t = array_t<storage_t::Scalar, storage_t::RowsAtCompileTime>;

      storage_t m_weights;
    };
    constexpr auto vertex_component_size(const vertex_component_bit_mask_et a_component) {
      switch (a_component) {
        using enum vertex_component_bit_mask_et;
        case e_position : return sizeof(geometry::vertex_position_st::array_t);
        case e_normal : return sizeof(geometry::vertex_normal_st::array_t);
        case e_tangent : return sizeof(geometry::vertex_tangent_st::array_t);
        case e_uv : return sizeof(geometry::vertex_uv_st::array_t);
        case e_color : return sizeof(geometry::vertex_color_st::array_t);
        case e_joints : return sizeof(geometry::vertex_joints_st::array_t);
        case e_weights : return sizeof(geometry::vertex_weights_st::array_t);
        default : contract_assert(false); std::unreachable();
      }
    }

    // concept modeling vertex components
    template <typename tp>
    concept vertex_component_c
      = mwc::concepts::any_of_c<tp, vertex_position_st, vertex_uv_st, vertex_normal_st, vertex_tangent_st, vertex_color_st,
                                vertex_joints_st, vertex_weights_st>;

    template <vertex_component_c... tp_vertex_components>
    struct vertex_st : public tp_vertex_components... {};

    // concept modeling specializations of [vertex_st]
    template <typename tp>
    concept vertex_c = specialization_of_v<tp, vertex_st>;

    // gltf vertex component identification
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
  }
}
export {
  // fastgltf vertex component specializations
  template <>
  struct fastgltf::ElementTraits<mwc::geometry::vertex_uv_st::array_t>
  : fastgltf::ElementTraitsBase<mwc::geometry::vertex_uv_st::array_t, AccessorType::Vec2,
                                mwc::geometry::vertex_uv_st::array_t::value_type> {};
  template <>
  struct fastgltf::ElementTraits<mwc::geometry::vertex_position_st::array_t>
  : fastgltf::ElementTraitsBase<mwc::geometry::vertex_position_st::array_t, AccessorType::Vec3,
                                mwc::geometry::vertex_position_st::array_t::value_type> {};
  template <>
  struct fastgltf::ElementTraits<mwc::geometry::vertex_tangent_st::array_t>
  : fastgltf::ElementTraitsBase<mwc::geometry::vertex_tangent_st::array_t, AccessorType::Vec4,
                                mwc::geometry::vertex_tangent_st::array_t::value_type> {};
  template <>
  struct fastgltf::ElementTraits<mwc::geometry::vertex_joints_st::array_t>
  : fastgltf::ElementTraitsBase<mwc::geometry::vertex_joints_st::array_t, AccessorType::Vec4,
                                mwc::geometry::vertex_joints_st::array_t::value_type> {};
}
