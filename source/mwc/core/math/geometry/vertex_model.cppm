module;

export module mwc_vertex_model;

import mwc_definition;
import mwc_math_definition;
import mwc_metaprogramming_utility;
import mwc_set_bit;
import mwc_concept;
import mwc_geometry;

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
      using vertex_position_t = position_t<>;

      vertex_position_t m_position;
    };
    struct vertex_normal_st {
      using vertex_normal_t = normal_t<>;
      vertex_normal_t m_normal;
    };
    struct vertex_tangent_st {
      using vertex_tangent_t = math::vector_t<vertex_normal_st::vertex_normal_t::Scalar, 4>;

      vertex_tangent_t m_tangent;
    };
    struct vertex_uv_st {
      using vertex_uv_t = math::vector_t<float32_t, 2>;

      vertex_uv_t m_uv;
    };
    struct vertex_color_st {
      using vertex_color_t = math::vector_t<float32_t, 3>;

      vertex_color_t m_color;
    };
    struct vertex_joints_st {
      using vertex_joints_t = math::vector_t<uint16_t, 4>;

      vertex_joints_t m_joints;
    };
    struct vertex_weights_st {
      using vertex_weights_t = math::vector_t<float32_t, 4>;

      vertex_weights_t m_weights;
    };
    constexpr auto vertex_component_size(const vertex_component_bit_mask_et a_component) {
      switch (a_component) {
        using enum vertex_component_bit_mask_et;
        case e_position : return sizeof(geometry::vertex_position_st::vertex_position_t);
        case e_normal : return sizeof(geometry::vertex_normal_st::vertex_normal_t);
        case e_tangent : return sizeof(geometry::vertex_tangent_st::vertex_tangent_t);
        case e_uv : return sizeof(geometry::vertex_uv_st::vertex_uv_t);
        case e_color : return sizeof(geometry::vertex_color_st::vertex_color_t);
        case e_joints : return sizeof(geometry::vertex_joints_st::vertex_joints_t);
        case e_weights : return sizeof(geometry::vertex_weights_st::vertex_weights_t);
        default : contract_assert(false); std::unreachable();
      }
    }

    // concept modeling vertex components
    template <typename tp>
    concept vertex_component_c =
      mwc::concepts::any_of_c<tp, vertex_position_st, vertex_uv_st, vertex_normal_st, vertex_tangent_st, vertex_color_st,
                              vertex_joints_st, vertex_weights_st>;

    template <vertex_component_c... tp_vertex_components>
    struct vertex_st : public tp_vertex_components... {};

    // concept modeling specializations of [vertex_st]
    template <typename tp>
    concept vertex_c = specialization_of_v<tp, vertex_st>;
  }
}
