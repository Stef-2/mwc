#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/instance.hpp"

import mwc_definition;
import mwc_static_string;
import mwc_empty_type;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class physical_device_ct : public handle_ct<vk::raii::PhysicalDevice> {
        public:
        using properties_t = vk::StructureChain<vk::PhysicalDeviceProperties2>;
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;
        };
        struct properties_st {
          using default_chain_t = vk::StructureChain<vk::PhysicalDeviceProperties2,
                                                     vk::PhysicalDeviceShaderObjectPropertiesEXT,
                                                     vk::PhysicalDeviceDescriptorIndexingProperties>;
          using memory_chain_t =
            vk::StructureChain<vk::PhysicalDeviceMemoryProperties2, vk::PhysicalDeviceMemoryBudgetPropertiesEXT>;
          using queue_family_chain_t =
            vk::StructureChain<vk::QueueFamilyProperties2 /*, vk::QueueFamilyOwnershipTransferPropertiesKHR*/>;

          default_chain_t m_default_properties_chain;
          memory_chain_t m_memory_properties_chain;
          vector_t<queue_family_chain_t> m_queue_family_properties_chain;
        };

        struct features_st {
          using default_chain_t = vk::StructureChain<vk::PhysicalDeviceFeatures2,
                                                     vk::PhysicalDeviceVulkan11Features,
                                                     vk::PhysicalDeviceVulkan12Features,
                                                     vk::PhysicalDeviceVulkan13Features,
                                                     vk::PhysicalDeviceVulkan14Features>;

          default_chain_t m_default_features_chain;
        };

        physical_device_ct(const instance_ct& a_instance,
                           const configuration_st& a_configuration = configuration_st::default_configuration());

        template <typename tp>
        [[nodiscard]] auto properties(this tp&& a_this) -> decltype(auto);
        template <typename tp>
        [[nodiscard]] auto features(this tp&& a_this) -> decltype(auto);
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        properties_st m_properties;
        features_st m_features;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto physical_device_ct::configuration_st::default_configuration() -> physical_device_ct::configuration_st {
        return configuration_st {};
      }
      template <typename tp>
      auto physical_device_ct::properties(this tp&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_properties);
      }
      template <typename tp>
      auto physical_device_ct::features(this tp&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_features);
      }
      template <typename tp_this>
      auto physical_device_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}