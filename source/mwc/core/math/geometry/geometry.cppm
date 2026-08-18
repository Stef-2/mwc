module;
/*
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
*/
export module mwc_geometry;

import mwc_definition;
import mwc_math_definition;

import glm;

import std;

export namespace mwc {
  namespace geometry {
    using default_scalar_t = float32_t;

    template <std::floating_point tp = default_scalar_t>
    using position_t = math::vector_t<tp, 3>;
    template <std::floating_point tp = default_scalar_t>
    using direction_t = math::vector_t<tp, 3>;
    template <std::floating_point tp = default_scalar_t>
    using orientation_t = math::quaternion_t<tp>;
    template <std::floating_point tp = default_scalar_t>
    using scale_t = math::vector_t<tp, 3>;
    template <std::floating_point tp = default_scalar_t, size_t tp_count = 4>
    using transformation_t = math::transformation_t<tp, tp_count>;
    using camera_projection_t = transformation_t<>;

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

    constexpr auto radians(const std::floating_point auto a_degrees) {
      using scalar_t = decltype(a_degrees);

      return a_degrees * std::numbers::pi_v<scalar_t> / scalar_t {180};
    }
    constexpr auto degrees(const std::floating_point auto a_radians) {
      using scalar_t = decltype(a_radians);

      return a_radians * scalar_t {180} / std::numbers::pi_v<scalar_t>;
    }
    constexpr auto coordinate_direction(const coordinate_direction_et a_coordinate_direction) -> direction_t<> {
      using enum coordinate_direction_et;
      switch (a_coordinate_direction) {
        case e_right : return direction_t<> {1.0, 0.0, 0.0};
        case e_left : return direction_t<> {-1.0, 0.0, 0.0};
        case e_up : return direction_t<> {0.0, 1.0, 0.0};
        case e_down : return direction_t<> {0.0, -1.0, 0.0};
        case e_forward : return direction_t<> {0.0, 0.0, 1.0};
        case e_backward : return direction_t<> {0.0, 0.0, -1.0};
        default : contract_assert(false); std::unreachable();
      }
    }
    constexpr auto identity() -> transformation_t<> {
      return transformation_t<> {static_cast<default_scalar_t>(1.0)};
    }
    constexpr auto identity() -> position_t<> {
      return position_t<> {0.0, 0.0, 0.0};
    }
    constexpr auto identity() -> scale_t<> {
      return scale_t<> {1.0, 1.0, 1.0};
    }

    // left handed
    template <std::floating_point tp = default_scalar_t>
    constexpr auto look_at(const position_t<tp>& a_origin, const position_t<tp>& a_target,
                           const coordinate_direction_et a_up_direction = coordinate_direction_et::e_up)
      -> transformation_t<tp, 3> {
      return glm::lookAt(a_origin, a_target, a_up_direction);
    }
    // left handed
    /*template <std::floating_point tp = default_scalar_t>
    constexpr auto
    look_at(const direction_t<tp>& a_direction, const coordinate_direction_et a_up_direction = coordinate_direction_et::e_up)
      -> orientation_t<tp> {
      const auto up_direction = coordinate_direction(a_up_direction);
      auto transformation = math::matrix_t<tp, 3, 3> {};

      const auto right = glm::cross(up_direction, a_direction);
      transformation.col(2) = a_direction;
      transformation.col(0) = right * (tp {1.0} / std::sqrt(std::max(tp {0.00001}, right.dot(right))));
      transformation.col(1) = transformation.col(2).cross(transformation.col(0));

      return orientation_t<tp> {transformation};
    }*/

    struct aabb_st {
      constexpr auto midpoint() const -> position_t<> {
        return (m_min + m_max) * static_cast<default_scalar_t> {0.5};
      }
      position_t<> m_min;
      position_t<> m_max;
    };
  }
}
