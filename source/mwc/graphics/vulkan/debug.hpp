#pragma once

#include "mwc/core/contract/definition.hpp"

import mwc_definition;
import mwc_debug;

import vulkan_hpp;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      template <typename tp>
        requires vk::isVulkanHandleType<typename tp::CppType>::value
      auto debug_name(const tp& a_vulkan_object, const string_view_t a_debug_name) -> void {
        if constexpr (diagnostic::debugging()) {
          const auto& device = a_vulkan_object.getDevice();
          const auto handle_id = reinterpret_cast<uintptr_t>(static_cast<tp::CType>(*a_vulkan_object));

          const auto debug_utils_object_name_info =
            vk::DebugUtilsObjectNameInfoEXT {tp::objectType, handle_id, a_debug_name.data()};

          const auto result = device.setDebugUtilsObjectNameEXT(debug_utils_object_name_info);
          contract_assert(result == vk::Result::eSuccess);
        }
      }
    }
  }
}