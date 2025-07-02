#pragma once

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/core/utility/semantic_version.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      struct context_st : public handle_ct<vk::raii::Context> {
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          semantic_version_st m_engine_version;
        };

        context_st(const configuration_st& a_configuration = configuration_st::default_configuration());

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