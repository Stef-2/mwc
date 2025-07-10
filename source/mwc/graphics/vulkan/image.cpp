#include "mwc/graphics/vulkan/image.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      image_ct::image_ct(const logical_device_ct& a_logical_device, const memory_allocator_ct& a_memory_allocator,
                         const configuration_st& a_configuration)
      : handle_ct {nullptr},
        m_allocation_info {},
        m_allocation {},
        m_configuration {a_configuration} {
        auto expected = a_memory_allocator.createImage(m_configuration.m_image_create_info,
                                                       m_configuration.m_allocation_create_info, m_allocation_info);
        contract_assert(expected.result == vk::Result::eSuccess);
        auto& [image, allocation] = expected.value;

        m_vulkan_handle = handle_t {a_logical_device.unique_handle(), image};
      }
    }
  }
}