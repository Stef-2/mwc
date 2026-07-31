module;

#include <vulkan/vulkan_hpp_macros.hpp>

export module mwc_vk_default_dispatcher;

import vulkan;

export namespace mwc {
  namespace graphics {
    namespace vulkan {
      constexpr auto default_dispatcher() -> decltype(auto) {
        return VULKAN_HPP_DEFAULT_DISPATCHER;
      }
    }
  }
}