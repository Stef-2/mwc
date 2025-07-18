module;

export module mwc_geometry;

import mwc_definition;
import mwc_math_definition;

import std;

export namespace mwc {
  namespace geometry {
    template <std::floating_point tp = float32_t>
    using position_t = math::vector_t<tp, 3>;
    template <std::floating_point tp = float32_t>
    using normal_t = math::vector_t<tp, 3>;
  }
}
