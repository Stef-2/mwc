#pragma once

#include "mwc/core/utility/semantic_version.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      struct context_st {
        context_st(const semantic_version_st& a_engine_version);

        vk::raii::Context m_context;
        semantic_version_st m_engine_version;
        semantic_version_st m_vulkan_api_version;
      };
    }
  }
}