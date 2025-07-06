#include "mwc/graphics/graphics.hpp"

namespace mwc {
  namespace graphics {

    graphics_ct::graphics_ct(const window_ct& a_window, const semantic_version_st& a_engine_version,
                             const configuration_st& a_configuration)
    : m_window {a_window},
      m_context {vulkan::context_st::configuration_st {.m_engine_version = a_engine_version}},
      m_instance {m_context},
      m_physical_device {m_instance},
      m_surface {m_window, m_instance, m_physical_device},
      m_queue_families {m_physical_device, m_surface},
      m_logical_device {m_physical_device, m_queue_families},
      m_memory_allocator {m_context, m_instance, m_physical_device, m_logical_device},
      m_swapchain {m_physical_device, m_surface, m_queue_families, m_logical_device, m_memory_allocator},
      m_configuration {a_configuration} {}
  }
}