#pragma once

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/queue_families.hpp"
#include "mwc/graphics/vulkan/command_pool.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class queue_ct : public handle_ct<vk::raii::Queue> {
        public:
        using fence_timeout_t = uint64_t;
        using command_buffer_count_t = uint32_t;

        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          command_pool_ct::configuration_st m_command_pool_configuration;
          command_buffer_count_t m_command_buffer_count;
          fence_timeout_t m_fence_timeout;
        };

        enum class queue_state_et { e_initial, e_recording, e_executing };

        queue_ct(const logical_device_ct& a_logical_device, const queue_families_ct& a_queue_families,
                 const configuration_st& a_configuration = configuration_st::default_configuration());

        auto command_pool() const -> const command_pool_ct&;
        auto command_buffers() const -> const span_t<const vk::raii::CommandBuffer>;
        auto queue_state() const -> queue_state_et;
        auto submit() -> void;
        auto submit_and_wait() -> void;

        protected:
        //auto clear() -> void;

        const logical_device_ct& m_logical_device;

        command_pool_ct m_command_pool;
        vector_t<vk::raii::CommandBuffer> m_command_buffers;
        queue_state_et m_queue_state;
        vk::raii::Fence m_submit_fence;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto queue_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {.m_command_pool_configuration = command_pool_ct::configuration_st::default_configuration(),
                                 .m_command_buffer_count = 1,
                                 .m_fence_timeout = std::numeric_limits<fence_timeout_t>::max()};
      }
    }
  }
}