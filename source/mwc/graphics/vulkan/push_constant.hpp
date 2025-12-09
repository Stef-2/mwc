#pragma once

#include "mwc/graphics/vulkan/pipeline_layout.hpp"

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
          array_t<vk::DeviceAddress, 8> m_registers = {};
        };
        struct material_data_st {
          using descriptor_index_t = pipeline_layout_ct::descriptor_count_t;

          descriptor_index_t m_base_color_map = {};
          descriptor_index_t m_metallic_roughness_map = {};
          descriptor_index_t m_normal_map = {};
        };

        geometry::transformation_t<> m_model = geometry::transformation_t<>::Identity();
        view_data_st m_view_data = {};
        projection_data_st m_projection_data = {};
        buffer_device_address_registers_st m_registers = {};
        material_data_st m_material_data = {};
        float64_t m_current_time = {};
        float64_t m_delta_time = {};
      };

      static_assert(sizeof(push_constant_st) <= 256);
    }
  }
}