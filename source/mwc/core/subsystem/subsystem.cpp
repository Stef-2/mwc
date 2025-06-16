module;

#include "mwc/core/diagnostic/log/default_log.hpp"

module mwc_subsystem;

namespace mwc {
  subsystem_st::subsystem_st(initializer_list_t<obs_ptr_t<subsystem_st>> a_dependencies, const string_view_t a_name)
  : m_dependencies {a_dependencies},
    m_name {a_name} {
    root_subsystem_st::s_subsystem_registry.push_back(this);
  }
  auto root_subsystem_st::initialize() -> void {
    std::print("initializing {0} subsystems", s_subsystem_registry.size() - 1);
  }
  auto root_subsystem_st::finalize() -> void {
    if constexpr (diagnostic::default_logging())
      mwc::information(std::format("finalizing {0} subsystems", s_subsystem_registry.size() - 1));
    else
      std::print("finalizing {0} subsystems", s_subsystem_registry.size() - 1);
  }
}