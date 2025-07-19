#include "mwc/ecs/subsystem.hpp"

namespace mwc {
  namespace ecs {
    auto ecs_subsystem_st::initialize() -> void {
      m_initialized = true;
    }
    auto ecs_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
  }
}