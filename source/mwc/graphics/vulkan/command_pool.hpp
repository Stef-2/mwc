#pragma once

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/queue_families.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class command_pool_ct : public handle_ct<vk::raii::CommandPool> {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          vk::CommandPoolCreateFlags m_command_pool_create_flags;
          vk::QueueFlags m_queue_flags;
        };

        command_pool_ct(const logical_device_ct& a_logical_device, const queue_families_ct& a_queue_families,
                        const configuration_st& a_configuration = configuration_st::default_configuration());

        auto queue_family_index() const -> queue_families_ct::family_st::index_t;

        private:
        queue_families_ct::family_st::index_t m_queue_family_index;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto command_pool_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {.m_command_pool_create_flags = {vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
                                 .m_queue_flags = vk::QueueFlagBits::eGraphics};
      }
    }
  }
}