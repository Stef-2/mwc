#include "mwc/graphics/vulkan/image.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      image_ct::image_ct(const logical_device_ct& a_logical_device, const memory_allocator_ct& a_memory_allocator,
                         const configuration_st& a_configuration)
      // initializes handle_ct, m_allocation_info and m_allocation
      : handle_ct {std::invoke([&a_logical_device, &a_memory_allocator, &a_configuration] -> handle_t {
          auto allocation_info = vma::AllocationInfo {};

          auto expected = a_memory_allocator.createImage(a_configuration.m_image_create_info,
                                                         a_configuration.m_allocation_create_info, allocation_info);
          contract_assert(expected.result == vk::Result::eSuccess);
          auto& [image, allocation] = expected.value;

          return handle_t {a_logical_device.unique_handle(), image};
        })},
        m_configuration {a_configuration} {}
    }
  }
}