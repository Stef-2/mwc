#include "mwc/core/chrono/subsystem.hpp"

namespace mwc {
  namespace chrono {
    auto chrono_subsystem_st::initialize() -> void {
      // logging subsystems is not operational at this point as it depends on this one
      // diagnostics will have to go through the standard output
      std::println("initializing {0}", m_name);
      initialization_time = high_resolution_clock_t::now();
      std::println("{0} initialized", m_name);
      m_initialized = true;
    }
    auto chrono_subsystem_st::finalize() -> void {
      std::println("finalizing {0}", m_name);
      std::println("{0} finalized", m_name);
      m_initialized = false;
    }
  }
}