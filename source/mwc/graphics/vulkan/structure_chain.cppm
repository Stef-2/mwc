module;

export module mwc_vulkan_structure_chain;

import mwc_definition;

import vulkan;

export namespace mwc {
  namespace graphics {
    namespace vulkan {
      template <typename... tps>
      struct structure_chain_st : public vk::StructureChain<tps...> {
        using vk_structure_chain_t = vk::StructureChain<tps...>;
        using storage_t = tuple_t<tps...>;

        using vk_structure_chain_t::vk_structure_chain_t;
        structure_chain_st(vk_structure_chain_t&& a_vk_structure_chain) : vk_structure_chain_t {a_vk_structure_chain} {}
      };
    }
  }
}