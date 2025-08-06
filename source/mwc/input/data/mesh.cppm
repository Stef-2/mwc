module;

export module mwc_input_mesh;

import mwc_definition;
import mwc_vertex_model;
import mwc_bit_mask;

import vulkan_hpp;

import std;

export namespace mwc {
  namespace input {
    template <geometry::vertex_c tp_vertex_model, std::unsigned_integral tp_index = uint32_t>
    struct static_mesh_st {
      using vertex_model_t = tp_vertex_model;
      using vertex_storage_t = vector_t<tp_vertex_model>;
      using index_storage_t = vector_t<tp_index>;

      static constexpr auto vk_index_type = vk::IndexTypeValue<tp_index>::value;

      vertex_storage_t m_vertex_storage;
      index_storage_t m_index_storage;
    };

    struct dynamic_mesh_st {
      using vertex_storage_t = vector_t<byte_t>;
      using index_storage_t = uint32_t;

      vertex_storage_t m_vertex_storage;
      index_storage_t m_index_storage;
      bit_mask_t<geometry::vertex_component_bit_mask_et> m_vertex_component_bit_mask;
    };
  }
}
