module;

export module mwc_geometry;

import mwc_definition;
import mwc_math_definition;

import std;

export namespace mwc {
  namespace geometry {
    using default_scalar_t = float32_t;

    template <std::floating_point tp = default_scalar_t>
    using position_t = math::vector_t<tp, 3>;
    template <std::floating_point tp = default_scalar_t>
    using normal_t = math::vector_t<tp, 3>;
    template <std::floating_point tp = default_scalar_t>
    using transformation_t = math::matrix_t<tp, 4, 4>;
  }
}
