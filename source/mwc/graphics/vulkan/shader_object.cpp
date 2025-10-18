#include "mwc/graphics/vulkan/shader_object.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {

      auto shader_object_binary_compatibility_st::assert_compatibility(const shader_object_binary_compatibility_st& a_other) const
        -> bool_t {
        return m_uuid == a_other.m_uuid and m_version >= a_other.m_version;
      }
    }
  }
}
