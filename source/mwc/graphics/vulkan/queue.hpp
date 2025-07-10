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

          queue_families_ct::family_st::index_t m_family_index;
        };

        enum class queue_state_et { e_initial, e_recording, e_executing };

        queue_ct(const logical_device_ct& a_logical_device,
                 const configuration_st& a_configuration = configuration_st::default_configuration());

        auto queue_state() const -> queue_state_et;
        auto submit() -> void;
        auto submit_and_wait() -> void;

        protected:
        //auto clear() -> void;

        const logical_device_ct& m_logical_device;

        queue_state_et m_queue_state;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto queue_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {.m_family_index = 0};
      }
    }
  }
}