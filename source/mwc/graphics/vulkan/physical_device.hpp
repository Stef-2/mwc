#pragma once

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/instance.hpp"

#include "vulkan/vulkan_core.h"

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

      template <typename... tp>
      consteval auto build_tuple() {
        tuple_t<vk::PhysicalDeviceProperties2> tup;
        tuple_t<tp...> tup2;
        /*using w = decltype(std::apply(
          [&tup](auto... x) {
            //static_assert(std::is_same_v<decltype(x), void>);
            if constexpr ((not std::is_same_v<decltype(x)..., empty_st>))
              return std::tuple_cat(decltype(tup) {}, std::tuple<decltype(x)...> {});
            else
              return;
          },
          tup2));*/
        using w =
          decltype(std::tuple_cat(tup, std::conditional_t<not std::is_same_v<tp, empty_st>, std::tuple<tp>, std::tuple<>> {}...));
        static_assert(std::is_same_v<w, void>);
        return w {};
      }
      using wtf_t = decltype(build_tuple<float, int, empty_st, char>());
      static_assert(not std::is_same_v<wtf_t, void>);

      template <auto... tp>
      class physical_device_template_ct : public handle_ct<vk::raii::PhysicalDevice> {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;
        };
        struct properties_st {
          tuple_t<vk::PhysicalDeviceProperties2, typename decltype(tp)::properties_t...> m_properties;
        };

        struct features_st {
          tuple_t<vk::PhysicalDeviceProperties2, typename decltype(tp)::features_t...> m_features;
        };

        physical_device_template_ct(const instance_ct& a_instance,
                                    const configuration_st& a_configuration = configuration_st::default_configuration());

        private:
        properties_st m_properties;
        features_st m_features;
        configuration_st m_configuration;
      };

      // required logical device extensions
      constexpr auto swapchain = logical_device_extension_st<empty_st, empty_st, VK_KHR_SWAPCHAIN_EXTENSION_NAME> {};
      constexpr auto buffer_device_address = logical_device_extension_st<empty_st,
                                                                         VkPhysicalDeviceBufferDeviceAddressFeaturesKHR,
                                                                         VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME> {};
      constexpr auto memory_budget =
        logical_device_extension_st<vk::PhysicalDeviceMemoryBudgetPropertiesEXT, empty_st, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME> {};
      constexpr auto memory_priority =
        logical_device_extension_st<empty_st, vk::PhysicalDeviceMemoryPriorityFeaturesEXT, VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME> {};
      constexpr auto dynamic_rendering =
        logical_device_extension_st<empty_st, vk::PhysicalDeviceDynamicRenderingFeaturesKHR, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME> {};
      constexpr auto shader_object = logical_device_extension_st<vk::PhysicalDeviceShaderObjectPropertiesEXT,
                                                                 vk::PhysicalDeviceShaderObjectFeaturesEXT,
                                                                 VK_EXT_SHADER_OBJECT_EXTENSION_NAME> {};

      using physical_device_ct =
        physical_device_template_ct<swapchain, memory_budget, memory_priority, dynamic_rendering, shader_object>;

      // implementation
      template <>
      constexpr auto physical_device_ct::configuration_st::default_configuration() -> physical_device_ct::configuration_st {
        return configuration_st {};
      }
    }
  }
}