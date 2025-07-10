#include "mwc/graphics/vulkan/memory_mapped_buffer.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      auto memory_mapped_buffer_ct::mapped_data_pointer() const -> void* {
        return m_allocation_info.pMappedData;
      }
      auto memory_mapped_buffer_ct::map() -> void {
        if (not mapped_data_pointer()) {
          const auto expected = m_memory_allocator.mapMemory(m_allocation);
          contract_assert(expected.result == vk::Result::eSuccess);

          m_allocation_info.pMappedData = expected.value;
        } else
          warning(std::format("attempting to memory map buffer {:#x} which has already been mapped", handle_id()));
      }
      auto memory_mapped_buffer_ct::unmap() -> void {
        if (mapped_data_pointer()) {
          m_memory_allocator.unmapMemory(m_allocation);
          m_allocation_info.pMappedData = nullptr;
        } else
          warning(std::format("attempting to memory unmap buffer {:#x} which has not been mapped", handle_id()));
      }
    }
  }
}