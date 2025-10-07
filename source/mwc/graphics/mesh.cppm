module;

#include "mwc/graphics/vulkan/buffer.hpp"

export module mwc_mesh;

import mwc_vertex_model;
import mwc_bit_mask;

import vulkan_hpp;

export namespace mwc {
  namespace graphics {
    struct mesh_st {
      vulkan::buffer_region_st m_vertex_buffer;
      bit_mask_t<geometry::vertex_component_bit_mask_et> m_vertex_component_bit_mask;
      size_t m_vertex_model_size;
      vulkan::buffer_region_st m_index_buffer;
      vk::IndexType m_index_type;
    };
  }
}