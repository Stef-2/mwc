module;

export module mwc_vertex_model;

import mwc_definition;
import mwc_math_definition;
import mwc_metaprogramming_utility;
import mwc_concept;
import mwc_geometry;

export namespace mwc {
  namespace geometry {
    // vertex components
    struct vertex_position_st {
      using vertex_position_t = position_t<>;

      vertex_position_t m_position;
    };
    struct vertex_uv_st {
      using vertex_uv_t = math::vector_t<float32_t, 2>;

      vertex_uv_t m_uv;
    };
    struct vertex_normal_st {
      using vertex_normal_t = normal_t<>;
      vertex_normal_t m_normal;
    };
    struct vertex_tangent_st {
      using vertex_tangent_t = normal_t<>;

      vertex_tangent_t m_tangent;
    };
    struct vertex_bitangent_st {
      using vertex_bitangent_t = normal_t<>;

      vertex_bitangent_t m_bitangent;
    };

    // concept modeling vertex components
    template <typename tp>
    concept vertex_component_c =
      mwc::concepts::any_of_c<tp, vertex_position_st, vertex_uv_st, vertex_normal_st, vertex_tangent_st, vertex_bitangent_st>;

    template <vertex_component_c... tp_vertex_components>
    struct vertex_st : public tp_vertex_components... {};

    // concept modeling specializations of [vertex_st]
    template <typename tp>
    concept vertex_c = specialization_of_v<tp, vertex_st>;
  }
}
