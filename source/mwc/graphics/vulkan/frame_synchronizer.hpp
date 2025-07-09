#pragma once

#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/debug.hpp"

import mwc_definition;
import mwc_debug;
import mwc_extent;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      template <size_t tp_frame_count = std::dynamic_extent>
        requires(tp_frame_count >= 2)
      struct frame_synchornizer_st {
        using frame_index_t = uint32_t;

        struct configuration_st {
          frame_index_t m_frame_count;
        };
        struct synchronization_st {
          vk::raii::Semaphore m_image_acquired_semaphore;
          vk::raii::Semaphore m_render_complete_semaphore;
          vk::raii::Fence m_frame_fence;
        };

        frame_synchornizer_st(const logical_device_ct& a_logical_device)
          requires(tp_frame_count != std::dynamic_extent);
        frame_synchornizer_st(const logical_device_ct& a_logical_device, const configuration_st& a_configuration)
          pre(a_configuration.m_frame_count >= 2)
          requires(tp_frame_count == std::dynamic_extent);

        extent_t<synchronization_st, tp_frame_count> m_synchronization_data;
        frame_index_t m_frame_index;
      };

      // implementation;
      template <size_t tp_frame_count>
        requires(tp_frame_count >= 2)
      frame_synchornizer_st<tp_frame_count>::frame_synchornizer_st(const logical_device_ct& a_logical_device)
        requires(tp_frame_count != std::dynamic_extent)
      : m_frame_index {0} {
        for (auto i = frame_index_t {0}; i < tp_frame_count; ++i) {
          m_synchronization_data[i] = synchronization_st {
            .m_image_acquired_semaphore =
              a_logical_device->createSemaphore(vk::SemaphoreCreateInfo {vk::SemaphoreCreateFlags {}}).value(),
            .m_render_complete_semaphore =
              a_logical_device->createSemaphore(vk::SemaphoreCreateInfo {vk::SemaphoreCreateFlags {}}).value(),
            .m_frame_fence = a_logical_device->createFence(vk::FenceCreateInfo {vk::FenceCreateFlagBits::eSignaled}).value()};

          if constexpr (diagnostic::debugging()) {
            const auto image_acquired_string = std::format("IA [{0}]", i);
            const auto render_complete_string = std::format("RC [{0}]", i);
            debug_name(m_synchronization_data[i].m_image_acquired_semaphore, image_acquired_string);
            debug_name(m_synchronization_data[i].m_render_complete_semaphore, render_complete_string);
          }
        }
      }
      template <size_t tp_frame_count>
        requires(tp_frame_count >= 2)
      frame_synchornizer_st<tp_frame_count>::frame_synchornizer_st(const logical_device_ct& a_logical_device,
                                                                   const configuration_st& a_configuration)
        requires(tp_frame_count == std::dynamic_extent)
      : m_frame_index {0} {
        m_synchronization_data.reserve(a_configuration.m_frame_count);

        for (auto i = frame_index_t {0}; i < a_configuration.m_frame_count; ++i) {
          m_synchronization_data.emplace_back(synchronization_st {
            a_logical_device->createSemaphore(vk::SemaphoreCreateInfo {vk::SemaphoreCreateFlags {}}).value(),
            a_logical_device->createSemaphore(vk::SemaphoreCreateInfo {vk::SemaphoreCreateFlags {}}).value(),
            a_logical_device->createFence(vk::FenceCreateInfo {vk::FenceCreateFlagBits::eSignaled}).value()});

          //if constexpr (diagnostic::debugging()) {
          const auto image_acquired_string = std::format("IA [{0}]", i);
          const auto render_complete_string = std::format("RC [{0}]", i);
          debug_name(m_synchronization_data.back().m_image_acquired_semaphore, image_acquired_string);
          debug_name(m_synchronization_data.back().m_render_complete_semaphore, render_complete_string);
          //}
        }
      }
    }
  }
}