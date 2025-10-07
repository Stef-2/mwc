#include "mwc/graphics/vulkan/suballocated_memory_mapped_buffer.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      suballocated_memory_mapped_buffer_ct::suballocated_memory_mapped_buffer_ct(const logical_device_ct& a_logical_device,
                                                                                 const memory_allocator_ct& a_memory_allocator,
                                                                                 const configuration_st& a_configuration)
      : memory_mapped_buffer_ct {
        a_logical_device, a_memory_allocator, a_configuration.m_memory_mapped_buffer_configuration,
      }, m_virtual_allocator {size(), a_configuration.m_virtual_allocator_configuration} {
      }
      auto suballocated_memory_mapped_buffer_ct::virtual_allocator() -> virtual_allocator_ct& {
        return m_virtual_allocator;
      }
    }
  }
}