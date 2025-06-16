module;

#include "mwc/core/diagnostic/log/subsystem.hpp"

module mwc_subsystem;

namespace mwc {
  subsystem_st::subsystem_st(initializer_list_t<obs_ptr_t<subsystem_st>> a_dependencies, const string_view_t a_name)
  : m_dependencies {a_dependencies},
    m_name {a_name} {
    root_subsystem_st::s_subsystem_registry.push_back(this);
  }
  subsystem_st::~subsystem_st() {
    std::erase(root_subsystem_st::s_subsystem_registry, this);
  }
  auto root_subsystem_st::initialize() -> void {
    std::println("initializing {0} subsystems", s_subsystem_registry.size() - 1);
    m_initialized = true;
  }
  auto root_subsystem_st::finalize() -> void {
    if constexpr (diagnostic::logging_subsystem_switch())
      mwc::information(std::format("finalizing {0} subsystems", s_subsystem_registry.size() - 1));
    else
      std::println("finalizing {0} subsystems", s_subsystem_registry.size() - 1);
    m_initialized = false;
  }
  auto initialize_subsystems() -> void {
    s_root_subsystem.initialize();

    for (auto& subsystem : root_subsystem_st::s_subsystem_registry) {
      auto dependencies_initialized = true;
      // check if all subsystems dependencies are initialized
      if (subsystem != &s_root_subsystem)
        for (const auto& dependency : subsystem->m_dependencies) {
          if (not dependency->m_initialized) {
            dependencies_initialized = false;
            break;
          }
          // if a subsystem is not initialized and all of its dependencies are, initialize
          if (not subsystem->m_initialized and dependencies_initialized)
            subsystem->initialize();
        }
    }
    // if there are any subsystems still left uninitialized, repeat
    for (auto& subsystem : root_subsystem_st::s_subsystem_registry)
      if (not subsystem->m_initialized)
        initialize_subsystems();
  }
  auto finalize_subsystems() -> void {
    for (auto& subsystem : root_subsystem_st::s_subsystem_registry)
      subsystem->finalize();
  }
}