#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/command_pool.hpp"
#include "mwc/graphics/vulkan/debug.hpp"

import mwc_definition;
import mwc_debug;
import mwc_extent;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      constexpr auto minimum_frame_count = 2;

      template <size_t tp_frame_count = std::dynamic_extent>
        requires(tp_frame_count >= minimum_frame_count)
      struct frame_synchornizer_st {
        using frame_count_t = uint64_t;
        using frame_index_t = uint32_t;

        struct configuration_st {
          frame_index_t m_frame_count;
        };
        struct synchronization_st {
          vk::raii::Semaphore m_image_acquired_semaphore = {nullptr};
          vk::raii::Semaphore m_render_complete_semaphore = {nullptr};
          vk::raii::CommandBuffer m_command_buffer = {nullptr};
        };

        // static constructor
        frame_synchornizer_st(const logical_device_ct& a_logical_device, const command_pool_ct& a_command_pool)
          requires(tp_frame_count != std::dynamic_extent);
        // dynamic constructor
        frame_synchornizer_st(const logical_device_ct& a_logical_device,
                              const command_pool_ct& a_command_pool,
                              const configuration_st& a_configuration) pre(a_configuration.m_frame_count >= minimum_frame_count)
          requires(tp_frame_count == std::dynamic_extent);
        auto frame_count() const -> frame_count_t;

        auto generate_data(const logical_device_ct& a_logical_device,
                           const command_pool_ct& a_command_pool,
                           const frame_index_t m_frame_count);

        extent_t<synchronization_st, tp_frame_count> m_synchronization_data;
        frame_count_t m_frame_count;
        frame_index_t m_frame_index;
        std::conditional_t<tp_frame_count == std::dynamic_extent, configuration_st, empty_st> m_configuration;
      };

      // implementation;
      template <size_t tp_frame_count>
        requires(tp_frame_count >= minimum_frame_count)
      frame_synchornizer_st<tp_frame_count>::frame_synchornizer_st(const logical_device_ct& a_logical_device,
                                                                   const command_pool_ct& a_command_pool)
        requires(tp_frame_count != std::dynamic_extent)
      : m_frame_count {0},
        m_frame_index {0},
        m_configuration {} {
        generate_data(a_logical_device, a_command_pool, tp_frame_count);
      }
      template <size_t tp_frame_count>
        requires(tp_frame_count >= minimum_frame_count)
      frame_synchornizer_st<tp_frame_count>::frame_synchornizer_st(const logical_device_ct& a_logical_device,
                                                                   const command_pool_ct& a_command_pool,
                                                                   const configuration_st& a_configuration)
        requires(tp_frame_count == std::dynamic_extent)
      : m_frame_count {0},
        m_frame_index {0},
        m_configuration {a_configuration} {
        generate_data(a_logical_device, a_command_pool, a_configuration.m_frame_count);
      }

      template <size_t tp_frame_count>
        requires(tp_frame_count >= minimum_frame_count)
      auto frame_synchornizer_st<tp_frame_count>::frame_count() const -> frame_count_t {
        if constexpr (tp_frame_count == std::dynamic_extent)
          return m_configuration.m_frame_count;
        else
          return tp_frame_count;
      }
      template <size_t tp_frame_count>
        requires(tp_frame_count >= minimum_frame_count)
      auto frame_synchornizer_st<tp_frame_count>::generate_data(const logical_device_ct& a_logical_device,
                                                                const command_pool_ct& a_command_pool,
                                                                const frame_index_t m_frame_count) {
        if constexpr (tp_frame_count == std::dynamic_extent)
          m_synchronization_data.resize(m_frame_count);

        auto command_buffers = a_logical_device->allocateCommandBuffers(
          vk::CommandBufferAllocateInfo {a_command_pool.native_handle(), vk::CommandBufferLevel::ePrimary, m_frame_count});
        contract_assert(command_buffers.result == vk::Result::eSuccess);

        for (auto i = frame_index_t {0}; i < m_frame_count; ++i) {
          m_synchronization_data[i] = synchronization_st {
            .m_image_acquired_semaphore
            = a_logical_device->createSemaphore(vk::SemaphoreCreateInfo {vk::SemaphoreCreateFlags {}}).value,
            .m_render_complete_semaphore
            = a_logical_device->createSemaphore(vk::SemaphoreCreateInfo {vk::SemaphoreCreateFlags {}}).value,
            .m_command_buffer = std::move(command_buffers.value[i])};

          if constexpr (diagnostic::debugging()) {
            const auto image_acquired_string = std::format("IA [{0}]", i);
            const auto render_complete_string = std::format("RC [{0}]", i);
            debug_name(m_synchronization_data[i].m_image_acquired_semaphore, image_acquired_string);
            debug_name(m_synchronization_data[i].m_render_complete_semaphore, render_complete_string);
          }
        }
      }
    }
  }
}