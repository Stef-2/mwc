module;

#include "mwc/graphics/vulkan/buffer.hpp"

export module mwc_device_mesh;

import mwc_observer_ptr;
import mwc_vertex_model;
import mwc_host_mesh;

import vulkan_hpp;

export namespace mwc {
  namespace graphics {
    struct device_mesh_st {
      vulkan::buffer_region_st m_vertex_buffer;
      vulkan::buffer_region_st m_index_buffer;
    };
  }
}