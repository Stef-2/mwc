#pragma once

#include "mwc/graphics/vulkan/suballocated_memory_mapped_buffer.hpp"

import mwc_definition;

import std;

namespace mwc {
  namespace graphics {
    class transfer_registry_st {
      using storage_t = vulkan::suballocated_memory_mapped_buffer_ct;

      const storage_t& m_common_buffer;
      vector_t<storage_t::suballocation_t<byte_t>> m_registered_transfers;
    };
  }
}
