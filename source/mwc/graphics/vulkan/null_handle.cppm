module;

#include <vulkan/vulkan_core.h>

export module mwc_vk_null_handle;

export namespace mwc {
  namespace graphics {
    namespace vulkan {
      inline constexpr auto null_handle = VK_NULL_HANDLE;
    }
  }
}