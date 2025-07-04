#pragma once

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/queue_families.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class logical_device_ct : public handle_ct<vk::raii::Device> {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          span_t<const char_t* const> m_required_extensions;
        };

        logical_device_ct(const physical_device_ct& a_physical_device, const queue_families_ct& a_queue_families,
                          const configuration_st& a_configuration = configuration_st::default_configuration());

        private:
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto logical_device_ct::configuration_st::default_configuration() -> configuration_st {
        static constexpr auto required_extensions = array_t<const char* const, 7> {
          vk::KHRSwapchainExtensionName,    vk::EXTSwapchainMaintenance1ExtensionName, vk::EXTMemoryBudgetExtensionName,
          vk::EXTShaderObjectExtensionName, vk::KHRMaintenance5ExtensionName,          vk::KHRMaintenance6ExtensionName,
          vk::KHRMaintenance7ExtensionName};

        return configuration_st {.m_required_extensions = required_extensions};
      }
    }
  }
}