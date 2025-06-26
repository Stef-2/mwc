#include "mwc/mwc.hpp"

namespace mwc {
  mwc_ct::mwc_ct(const configuration_st& a_configuration)
  : m_window {a_configuration.m_window_configuration},
    m_graphics {m_window, a_configuration.m_version},
    m_configuration {a_configuration} {}
  mwc_ct::~mwc_ct() {
    // if there are any subsystems left unfinalized, finalize them
    for (auto& subsystem : subsystem_st::subsystem_registry)
      if (subsystem->m_initialized) {
        finalize_subsystems();
        return;
      }
  }
}