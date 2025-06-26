#include "mwc/graphics/vulkan/context.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {

      context_st::context_st(const semantic_version_st& a_engine_version)
      : m_context {},
        m_engine_version {a_engine_version},
        m_vulkan_api_version {m_context.enumerateInstanceVersion()} {}
    }
  }
}