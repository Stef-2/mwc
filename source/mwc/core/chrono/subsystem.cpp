#include "mwc/core/chrono/subsystem.hpp"

namespace mwc {
  namespace chrono {
    auto chrono_subsystem_st::initialize() -> void {
      initialization_time = high_resolution_clock_t::now();
      m_initialized = true;
    }
    auto chrono_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
  }
}