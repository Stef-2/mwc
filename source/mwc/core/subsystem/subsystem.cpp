module;

#include "mwc/core/diagnostic/log/subsystem.hpp"

module mwc_subsystem;

namespace mwc {
  subsystem_st::subsystem_st(const initializer_list_t<obs_ptr_t<subsystem_st>> a_dependencies, const string_view_t a_name)
  : m_dependencies {a_dependencies},
    m_name {a_name},
    m_initialized {false} {
    subsystem_registry.push_back(this);
  }
  subsystem_st::~subsystem_st() {
    std::erase(subsystem_registry, this);
  }
  auto subsystem_st::depends_on(const subsystem_st& a_subsystem) -> const bool {
    auto dependency_found = false;
    auto search_for_dependency = [&dependency_found, &a_subsystem](this auto&& a_this,
                                                                   const span_t<subsystem_st*> a_dependencies) -> void {
      for (const auto& dependency : a_dependencies) {
        if (dependency == &a_subsystem) {
          dependency_found = true;
          return;
        }
        a_this(dependency->m_dependencies);
      }
    };
    search_for_dependency(m_dependencies);
    return dependency_found;
  }
  auto initialize_subsystems() -> void {
    std::println("initializing {0} subsystems", subsystem_st::subsystem_registry.size());

    const auto initializer = [](this auto&& a_this) -> void {
      for (auto& subsystem : subsystem_st::subsystem_registry) {
        auto dependencies_initialized = true;
        // check if all subsystems dependencies are initialized
        for (const auto& dependency : subsystem->m_dependencies) {
          if (not dependency->m_initialized) {
            dependencies_initialized = false;
            break;
          }
        }
        // if a subsystem is not initialized but all of its dependencies are, initialize
        if (not subsystem->m_initialized and dependencies_initialized) {
          const auto logging_subsystem_initialized = diagnostic::log::global::logging_subsystem.m_initialized;
          if (logging_subsystem_initialized)
            information(std::format("initializing {0}", subsystem->m_name));
          else
            std::println("initializing {0}", subsystem->m_name);
          subsystem->initialize();
          if (logging_subsystem_initialized)
            information(std::format("{0} initialized", subsystem->m_name));
          else
            std::println("{0} initialized", subsystem->m_name);
        }
      }
      // if there are any subsystems still left uninitialized, repeat
      for (auto& subsystem : subsystem_st::subsystem_registry)
        if (not subsystem->m_initialized)
          a_this();
    };

    initializer();
    std::println("{0} subsystems initialized", subsystem_st::subsystem_registry.size());
  }
  auto finalize_subsystems() -> void {
    std::println("finalizing {0} subsystems", subsystem_st::subsystem_registry.size());
    for (auto& subsystem : subsystem_st::subsystem_registry)
      subsystem->finalize();

    std::println("{0} subsystems finalized", subsystem_st::subsystem_registry.size());
  }
}