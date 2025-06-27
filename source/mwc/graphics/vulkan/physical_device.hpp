#pragma once

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
      template <typename tp_properties, typename tp_features, static_string_st tp_name>
      struct logical_device_extension_st {

        using properties_t = tp_properties;
        using features_t = tp_features;
        static constexpr inline auto name = tp_name;
      };
      template <auto... tp>
      class physical_device_template_ct : public handle_ct<vk::raii::PhysicalDevice> {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;
        };
        struct properties_st {
          // generate the properties tuple type from the passed template parameters
          // prepend vk::PhysicalDeviceProperties2 and filter out any empty_st types
          using properties_tuple_t =
            decltype(std::tuple_cat(tuple_t<vk::PhysicalDeviceProperties2> {},
                                    std::conditional_t<(not std::is_same_v<typename decltype(tp)::properties_t, empty_st>),
                                                       std::tuple<typename decltype(tp)::properties_t>,
                                                       std::tuple<>> {}...));
          properties_tuple_t m_properties;
        };

        struct features_st {
          // generate the features tuple type from the passed template parameters
          // prepend vk::PhysicalDeviceFeatures2 and filter out any empty_st types
          using features_tuple_t =
            decltype(std::tuple_cat(tuple_t<vk::PhysicalDeviceFeatures2> {},
                                    std::conditional_t<(not std::is_same_v<typename decltype(tp)::features_t, empty_st>),
                                                       std::tuple<typename decltype(tp)::features_t>,
                                                       std::tuple<>> {}...));
          features_tuple_t m_features;
        };

        physical_device_template_ct(const instance_ct& a_instance,
                                    const configuration_st& a_configuration = configuration_st::default_configuration());

        private:
        properties_st m_properties;
        features_st m_features;
        configuration_st m_configuration;
      };

      constexpr auto name = string_view_t {vk::KHRSwapchainExtensionName};
      // required logical device extensions
      constexpr auto swapchain = logical_device_extension_st<empty_st, empty_st, {vk::KHRSwapchainExtensionName}> {};
      constexpr auto buffer_device_address =
        logical_device_extension_st<empty_st, vk::PhysicalDeviceBufferDeviceAddressFeatures, {vk::KHRBufferDeviceAddressExtensionName}> {};
      constexpr auto memory_budget =
        logical_device_extension_st<vk::PhysicalDeviceMemoryBudgetPropertiesEXT, empty_st, {vk::EXTMemoryBudgetExtensionName}> {};
      constexpr auto memory_priority =
        logical_device_extension_st<empty_st, vk::PhysicalDeviceMemoryPriorityFeaturesEXT, {vk::EXTMemoryPriorityExtensionName}> {};
      constexpr auto dynamic_rendering =
        logical_device_extension_st<empty_st, vk::PhysicalDeviceDynamicRenderingFeaturesKHR, {vk::KHRDynamicRenderingExtensionName}> {};
      constexpr auto shader_object = logical_device_extension_st<vk::PhysicalDeviceShaderObjectPropertiesEXT,
                                                                 vk::PhysicalDeviceShaderObjectFeaturesEXT,
                                                                 {vk::EXTShaderObjectExtensionName}> {};

      // physical device specialization
      using physical_device_ct =
        physical_device_template_ct<swapchain, buffer_device_address /*, memory_budget*/, memory_priority, dynamic_rendering, shader_object>;

      // implementation
      template <>
      constexpr auto physical_device_ct::configuration_st::default_configuration() -> physical_device_ct::configuration_st {
        return configuration_st {};
      }
    }
  }
}