#pragma once

#include "mwc/core/container/data_span.hpp"

import mwc_input_resource;
import mwc_host_mesh;
import mwc_device_mesh;

namespace mwc {
  namespace input {
    struct mesh_st : public resource_st {
      using suballocation_t = data_span_st<byte_t>;

      dynamic_host_mesh_st m_host_mesh;
      suballocation_t m_memory_mapped_suballocation;
      graphics::device_mesh_st m_device_mesh;
    };
  }
}