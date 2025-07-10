#include "mwc/core/memory/virtual_allocator.hpp"

import vulkan_hpp;

namespace mwc {
  virtual_allocator_ct::virtual_allocator_ct(const memory_size_t a_virtual_memory_block_size,
                                             const configuration_st& a_configuration)
  : m_virtual_memory_block {vma::createVirtualBlock(vma::VirtualBlockCreateInfo {a_virtual_memory_block_size,
                                                                                 a_configuration.m_virtual_block_create_flags})
                              .value},
    m_virtual_allocation_map {} {}
  virtual_allocator_ct::~virtual_allocator_ct() {
    m_virtual_memory_block.destroy();
  }
  auto virtual_allocator_ct::acquire_suballocation(const memory_size_t a_virtual_suballocation_size,
                                                   const allocation_configuration_st& a_configuration) -> memory_offset_t {
    const auto virtual_allocation_create_info = vma::VirtualAllocationCreateInfo {
      a_virtual_suballocation_size, a_configuration.m_alignment, a_configuration.m_virtual_allocation_create_flags};
    auto offset = std::numeric_limits<memory_offset_t>::max();
    auto allocation = vma::VirtualAllocation {};

    const auto result = m_virtual_memory_block.virtualAllocate(&virtual_allocation_create_info, &allocation, &offset);
    contract_assert(result == vk::Result::eSuccess);

    m_virtual_allocation_map.emplace(offset, allocation);

    return offset;
  }
  auto virtual_allocator_ct::free_suballocation(const memory_offset_t a_virtual_memory_offset) -> void {
    m_virtual_memory_block.virtualFree(m_virtual_allocation_map[a_virtual_memory_offset]);
    m_virtual_allocation_map.erase(a_virtual_memory_offset);
  }
}
