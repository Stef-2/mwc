module;

#include "mwc/core/contract/definition.hpp"

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

    enum class coordinate_axis_et : uint8_t {
      e_x,
      e_y,
      e_z,
      end
    };
    enum class coordinate_direction_et : uint8_t {
      e_right,
      e_left,
      e_up,
      e_down,
      e_forward,
      e_backward,
      end
    };
    constexpr auto coordinate_direction(const coordinate_direction_et a_coordinate_direction) -> normal_t<> {
      using enum coordinate_direction_et;
      switch (a_coordinate_direction) {
        case e_right : return normal_t<> {1.0, 0.0, 0.0};
        case e_left : return normal_t<> {-1.0, 0.0, 0.0};
        case e_up : return normal_t<> {0.0, 1.0, 0.0};
        case e_down : return normal_t<> {0.0, -1.0, 0.0};
        case e_forward : return normal_t<> {0.0, 0.0, 1.0};
        case e_backward : return normal_t<> {0.0, 0.0, -1.0};
        default : contract_assert(false); std::unreachable();
      }
    }
    constexpr auto radians(const std::floating_point auto a_degrees) {
      using scalar_t = decltype(a_degrees);

      return a_degrees * std::numbers::pi_v<scalar_t> / scalar_t {180};
    }
    constexpr auto degrees(const std::floating_point auto a_radians) {
      using scalar_t = decltype(a_radians);

      return a_radians * scalar_t {180} / std::numbers::pi_v<scalar_t>;
    }

    struct aabb_st {
      position_t<> m_min;
      position_t<> m_max;
    };
  }
}
