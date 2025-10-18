#include "mwc/graphics/vulkan/context.hpp"

#include "vulkan/vulkan_hpp_macros.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      context_st::context_st(const configuration_st& a_configuration)
      : handle_ct {handle_ct::handle_t {}},
        m_engine_version {a_configuration.m_engine_version},
        m_vulkan_api_version {m_context.enumerateInstanceVersion().value} {
        // basic initialization of vulkan dynamic dispatcher
        VULKAN_HPP_DEFAULT_DISPATCHER.init();
      }
    }
  }
}