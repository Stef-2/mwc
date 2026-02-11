module;

#include "mwc/core/diagnostic/assert.hpp"
/*
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
*/
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
    using direction_t = math::vector_t<tp, 3>;
    template <std::floating_point tp = default_scalar_t>
    using orientation_t = math::quaternion_t<tp>;
    template <std::floating_point tp = default_scalar_t>
    using scale_t = math::vector_t<tp, 3>;
    template <std::floating_point tp = default_scalar_t, size_t tp_count = 3,
              math::dense_storage_type_et tp_storage_type = math::dense_storage_type_et::e_affine,
              math::dense_storage_order_et tp_storage_order = math::dense_storage_order_et::e_column_major>
    using transformation_t = math::transformation_t<tp, tp_count, tp_storage_type, tp_storage_order>;
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
    // left handed
    template <std::floating_point tp = default_scalar_t>
    constexpr auto look_at(const position_t<tp>& a_origin, const position_t<tp>& a_target,
                           const coordinate_direction_et a_up_direction = coordinate_direction_et::e_up)
      -> transformation_t<tp, 3> {
      const auto up_direction = coordinate_direction(a_up_direction);

      const auto forward = direction_t<tp> {a_origin - a_target}.normalized();
      const auto right = direction_t<tp> {up_direction.cross(forward)}.normalized();
      const auto up = direction_t<tp> {forward.cross(right)};

      auto transformation = transformation_t<tp, 3>::Identity();
      transformation(0, 0) = right.x();
      transformation(0, 1) = right.y();
      transformation(0, 2) = right.z();
      transformation(1, 0) = up.x();
      transformation(1, 1) = up.y();
      transformation(1, 2) = up.z();
      transformation(2, 0) = forward.x();
      transformation(2, 1) = forward.y();
      transformation(2, 2) = forward.z();
      transformation(0, 3) = -right.dot(a_origin);
      transformation(1, 3) = -up.dot(a_origin);
      transformation(2, 3) = -forward.dot(a_origin);

      return transformation;
    }
    // left handed
    template <std::floating_point tp = default_scalar_t>
    constexpr auto
    look_at(const direction_t<tp>& a_direction, const coordinate_direction_et a_up_direction = coordinate_direction_et::e_up)
      -> orientation_t<tp> {
      const auto up_direction = coordinate_direction(a_up_direction);
      auto transformation = math::matrix_t<tp, 3, 3> {};

      const auto right = up_direction.cross(a_direction);
      transformation.col(2) = a_direction;
      transformation.col(0) = right * (tp {1.0} / std::sqrt(std::max(tp {0.00001}, right.dot(right))));
      transformation.col(1) = transformation.col(2).cross(transformation.col(0));

      return orientation_t<tp> {transformation};
    }
    constexpr auto coordinate_orientation(const coordinate_direction_et a_coordinate_direction) -> orientation_t<> {
      //glm::lookAtLH()
      // glm::quatLookAtLH()
      /*
      const auto up = coordinate_direction(coordinate_direction_et::e_up);
      const auto fwd = direction_t<> {0.0, 0.0, 1.0};
      const auto cross = up.cross(coordinate_direction(a_coordinate_direction)).normalized();
      return orientation_t<> {math::angle_axis_t<orientation_t<>::Scalar> {radians(90.0), fwd}}
           * math::angle_axis_t<orientation_t<>::Scalar> {90.0, up};*/
      auto t = look_at({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}).rotation();
      auto t2 = look_at({0.0f, 0.0f, 1.0f});

      orientation_t<> o = {0.339851, 0.17592, 0.820473, 0.424708};
      o.w() *= -1.0;
      return o;
    }

    struct aabb_st {
      position_t<> m_min;
      position_t<> m_max;
    };
  }
}
