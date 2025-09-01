#pragma once

#include "mwc/ecs/entity.hpp"
#include "mwc/ecs/component_convergence.hpp"

import mwc_definition;
import mwc_math_definition;
import mwc_geometry;
import mwc_camera_projection_type;

import std;

namespace mwc {
  namespace graphics {
    class camera_ct : public ecs::entity_ct {
      public:
      using components_tuple_t = tuple_t<ecs::transformation_st, ecs::camera_projection_st, ecs::camera_type_st>;
      using scalar_t = ecs::camera_projection_st::underlying_pod_t;
      using projection_t = decltype(std::declval<ecs::camera_projection_st>().m_projection);

      template <camera_projection_et tp_camera_projection = camera_projection_et::e_perspective>
      struct configuration_st {};
      template <>
      struct configuration_st<camera_projection_et::e_perspective> {
        static constexpr auto default_configuration() -> configuration_st<camera_projection_et::e_perspective>;

        scalar_t m_aspect_ratio;
        scalar_t m_field_of_view_degrees;
        scalar_t m_near_clip;
        scalar_t m_far_clip;
      };
      template <>
      struct configuration_st<camera_projection_et::e_orthographic> {
        static constexpr auto default_configuration() -> configuration_st<camera_projection_et::e_orthographic>;

        scalar_t m_left_clip;
        scalar_t m_right_clip;
        scalar_t m_bottom_clip;
        scalar_t m_top_clip;
        scalar_t m_near_clip;
        scalar_t m_far_clip;
      };
      struct test_cfg {
        scalar_t m_aspect_ratio;
        scalar_t m_field_of_view_degrees;
        scalar_t m_near_clip;
        scalar_t m_far_clip;
      };

      template <camera_projection_et tp_camera_projection = camera_projection_et::e_perspective>
      constexpr camera_ct(const configuration_st<tp_camera_projection>& a_configuration
                          = configuration_st<camera_projection_et::e_perspective>::default_configuration());

      private:
      template <camera_projection_et tp_camera_projection>
      static constexpr auto generate_projection_matrix(const configuration_st<tp_camera_projection>& a_configuration)
        -> projection_t;
    };

    // implementation
    constexpr auto camera_ct::configuration_st<camera_projection_et::e_perspective>::default_configuration()
      -> configuration_st<camera_projection_et::e_perspective> {
      return configuration_st<camera_projection_et::e_perspective> {.m_aspect_ratio = 16.0 / 9.0,
                                                                    .m_field_of_view_degrees = 90.0,
                                                                    .m_near_clip = 0.1,
                                                                    .m_far_clip = 1024.0};
    }
    constexpr auto camera_ct::configuration_st<camera_projection_et::e_orthographic>::default_configuration()
      -> configuration_st<camera_projection_et::e_orthographic> {
      return configuration_st<camera_projection_et::e_orthographic> {.m_left_clip = -512.0,
                                                                     .m_right_clip = 512.0,
                                                                     .m_bottom_clip = -512.0,
                                                                     .m_top_clip = 512.0,
                                                                     .m_near_clip = -512.0,
                                                                     .m_far_clip = 512.0};
    }
    template <camera_projection_et tp_camera_projection>
    constexpr camera_ct::camera_ct(const configuration_st<tp_camera_projection>& a_configuration)
    : ecs::entity_ct {ecs::transformation_st {.m_transformation = geometry::transformation_t<>::Identity()},
                      ecs::camera_projection_st {.m_projection = generate_projection_matrix(a_configuration)},
                      ecs::camera_type_st {.m_projection_type = tp_camera_projection}} {}
    template <camera_projection_et tp_camera_projection>
    constexpr auto camera_ct::generate_projection_matrix(const configuration_st<tp_camera_projection>& a_configuration)
      -> projection_t {
      if constexpr (tp_camera_projection == camera_projection_et::e_perspective) {
        const auto half_fov_tangent
          = scalar_t {std::tan(geometry::radians(a_configuration.m_field_of_view_degrees) / scalar_t {2.0})};

        return projection_t {{scalar_t {1.0} / (a_configuration.m_aspect_ratio * half_fov_tangent), 0.0, 0.0, 0.0},
                             {0.0, scalar_t {1.0} / half_fov_tangent, 0.0, 0.0},
                             {0.0, 0.0,
                              -(a_configuration.m_far_clip + a_configuration.m_near_clip)
                                / (a_configuration.m_far_clip - a_configuration.m_near_clip),
                              scalar_t {-2.0} * a_configuration.m_far_clip * a_configuration.m_near_clip
                                / (a_configuration.m_far_clip - a_configuration.m_near_clip)},
                             {0.0, 0.0, -1.0, 0.0}};
      } else {
        const auto width = a_configuration.m_right_clip - a_configuration.m_left_clip;
        const auto height = a_configuration.m_top_clip - a_configuration.m_bottom_clip;
        const auto depth = a_configuration.m_far_clip - a_configuration.m_near_clip;

        return projection_t {
          2.0 / width, 0.0,          0.0,          -(a_configuration.m_right_clip + a_configuration.m_left_clip) / width,
          0.0,         2.0 / height, 0.0,          -(a_configuration.m_top_clip + a_configuration.m_bottom_clip) / height,
          0.0,         0.0,          -2.0 / depth, -(a_configuration.m_far_clip + a_configuration.m_near_clip) / depth,
          0.0,         0.0,          0.0,          1.0};
      }
    }
  }
}