#pragma once

import mwc_math_definition;
import mwc_geometry;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      struct push_constant_st {
        struct view_data_st {
          math::vector_t<float32_t, 4> m_view_position = {};
          math::vector_t<float32_t, 4> m_view_direction = {};
        };
        struct projection_data_st {
          vk::Extent2D m_window_extent = {};
          float32_t m_aspect_ratio = {};
          float32_t m_near_clip = {};
          float32_t m_far_clip = {};
        };
        struct buffer_device_address_registers_st {
          vk::DeviceAddress m_register_0 = {};
          vk::DeviceAddress m_register_1 = {};
          vk::DeviceAddress m_register_2 = {};
          vk::DeviceAddress m_register_3 = {};
          vk::DeviceAddress m_register_4 = {};
          vk::DeviceAddress m_register_5 = {};
          vk::DeviceAddress m_register_6 = {};
          vk::DeviceAddress m_register_7 = {};
        };

        geometry::transformation_t<> m_model = geometry::transformation_t<>::Identity();
        view_data_st m_view_data = {};
        projection_data_st m_projection_data = {};
        buffer_device_address_registers_st m_registers = {};
        float64_t m_current_time = {};
        float64_t m_delta_time = {};
      };

      static_assert(sizeof(push_constant_st) <= 256);
    }
  }
}