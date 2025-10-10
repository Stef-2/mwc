module;

export module mwc_host_mesh;

import mwc_definition;
import mwc_geometry;
import mwc_vertex_model;
import mwc_bit_mask;

import vulkan_hpp;

import std;

export namespace mwc {
  namespace input {
    // mesh using a static vertex model
    template <geometry::vertex_c tp_vertex_model, std::unsigned_integral tp_index = uint32_t>
    struct static_host_mesh_st {
      using name_t = string_t;
      using vertex_model_t = tp_vertex_model;
      using vertex_storage_t = vector_t<tp_vertex_model>;
      using index_storage_t = vector_t<tp_index>;

      static constexpr auto vk_index_type = vk::IndexTypeValue<tp_index>::value;

      vertex_storage_t m_vertex_storage;
      index_storage_t m_index_storage;
      geometry::aabb_st m_aabb;
    };
    // mesh using a dynamic vertex model
    struct dynamic_host_mesh_st {
      using name_t = string_t;
      using vertex_storage_t = vector_t<byte_t>;
      using index_storage_t = vector_t<uint32_t>;

      vertex_storage_t m_vertex_storage;
      index_storage_t m_index_storage;
      bit_mask_t<geometry::vertex_component_bit_mask_et> m_vertex_component_bit_mask;
      size_t m_vertex_model_size;
      size_t m_vertex_count;
      size_t m_index_count;
      geometry::aabb_st m_aabb;
    };
  }
}
