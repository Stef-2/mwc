#include "mwc/graphics/graphics.hpp"

namespace mwc {
  namespace graphics {

    graphics_ct::graphics_ct(const window_ct& a_window, const semantic_version_st& a_engine_version,
                             const configuration_st& a_configuration)
    : m_window {a_window},
      m_context {a_engine_version},
      m_instance {m_context},
      m_physical_device {m_instance},
      m_surface {m_window, m_instance, m_physical_device},
      m_configuration {a_configuration} {}
  }
}