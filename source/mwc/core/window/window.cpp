#include "mwc/core/window/window.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace mwc {
  window_ct::window_ct(const configuration_st& a_configuration)
  : m_monitor {vkfw::getPrimaryMonitor().value},
    m_window {vkfw::createWindowUnique(a_configuration.m_resolution.m_width, a_configuration.m_resolution.m_height,
                                       a_configuration.m_title.data(), a_configuration.m_window_hints,
                                       a_configuration.m_full_screen ? m_monitor : nullptr)
                .value},
    m_configuration {a_configuration} {
    information(std::format("created window"));
  }
}