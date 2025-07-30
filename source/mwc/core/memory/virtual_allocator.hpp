#pragma once

#include "mwc/core/contract/definition.hpp"

import mwc_definition;
import mwc_type_mobility;

import vk_mem_alloc_hpp;

import std;

namespace mwc {
  class virtual_allocator_ct : public irreproducible_st {
    public:
    using memory_offset_t = size_t;
    using memory_size_t = size_t;
    using memory_alignment_t = size_t;

    static constexpr auto failed_suballocation_offset = std::numeric_limits<memory_offset_t>::max();

    struct configuration_st {
      static constexpr auto default_configuration() -> const configuration_st;

      vma::VirtualBlockCreateFlags m_virtual_block_create_flags;
    };

    struct allocation_configuration_st {
      static constexpr auto default_configuration() -> const allocation_configuration_st;

      vma::VirtualAllocationCreateFlags m_virtual_allocation_create_flags;
      memory_alignment_t m_alignment;
    };

    virtual_allocator_ct(const memory_size_t a_virtual_memory_block_size,
                         const configuration_st& a_configuration = configuration_st::default_configuration())
      pre(a_virtual_memory_block_size > 0);
    ~virtual_allocator_ct();

    [[nodiscard]] auto request_suballocation(
      const memory_size_t a_virtual_suballocation_size,
      const allocation_configuration_st& a_configuration = allocation_configuration_st::default_configuration())
      -> memory_offset_t pre(a_virtual_suballocation_size > 0) post(r : r != failed_suballocation_offset);
    auto release_suballocation(const memory_offset_t a_virtual_memory_offset)
      -> void pre(m_virtual_allocation_map.contains(a_virtual_memory_offset));
    auto clear_virtual_memory_block() const -> void;

    private:
    vma::VirtualBlock m_virtual_memory_block;
    map_t<memory_offset_t, vma::VirtualAllocation> m_virtual_allocation_map;
  };

  // implementation
  constexpr auto virtual_allocator_ct::configuration_st::default_configuration() -> const configuration_st {
    return configuration_st {.m_virtual_block_create_flags = vma::VirtualBlockCreateFlags {}};
  }
  constexpr auto virtual_allocator_ct::allocation_configuration_st::default_configuration() -> const allocation_configuration_st {
    return allocation_configuration_st {.m_virtual_allocation_create_flags = vma::VirtualAllocationCreateFlags {},
                                        .m_alignment = 0};
  }
}