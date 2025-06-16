#include "mwc/core/window/subsystem.hpp"

namespace mwc {
  auto window_subsystem_st::initialize() -> void {
    information("initializing window subsystem");
    const auto vkfw_initialization = vkfw::init();
    contract_assert(vkfw::check(vkfw_initialization));
    information("window subsystem initialized");
    m_initialized = true;
  }
  auto window_subsystem_st::finalize() -> void {
    information("finalizing window subsystem");
    std::ignore = vkfw::terminate();
    information("window subsystem finalized");
    m_initialized = false;
  }
}