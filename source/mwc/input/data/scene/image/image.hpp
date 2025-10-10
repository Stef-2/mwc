#pragma once

#include "mwc/core/container/data_span.hpp"

import mwc_input_resource;
import mwc_host_image;
import mwc_device_image;

namespace mwc {
  namespace input {
    struct image_st : public resource_st {
      using suballocation_t = data_span_st<byte_t>;

      dynamic_host_image_st m_host_image;
      suballocation_t m_memory_mapped_suballocation;
      graphics::device_image_st m_device_image;
    };
  }
}