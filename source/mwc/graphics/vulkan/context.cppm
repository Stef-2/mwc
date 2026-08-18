module;

export module mwc_vk_context;

import mwc_definition;
import mwc_semantic_version;
import mwc_vk_handle;

import vulkan;

import std;

export namespace mwc {
  namespace graphics {
    namespace vulkan {
      struct context_st : public handle_ct<vk::raii::Context> {
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          semantic_version_st m_engine_version;
        };

        context_st(const configuration_st& a_configuration = configuration_st::default_configuration());

        // note: what v
        vk::raii::Context m_context;
        semantic_version_st m_engine_version;
        semantic_version_st m_vulkan_api_version;
      };

      // implementation
      constexpr auto context_st::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {.m_engine_version = {0, 3, 1}};
      }
    }
  }
}