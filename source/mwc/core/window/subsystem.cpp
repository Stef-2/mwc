#include "mwc/core/window/subsystem.hpp"

namespace mwc {
  auto window_subsystem_st::initialize() -> void {
    information(std::format("initializing {0}", m_name));
    const auto vkfw_initialization = vkfw::init();
    contract_assert(vkfw::check(vkfw_initialization));
    information(std::format("{0} initialized", m_name));
    m_initialized = true;
  }
  auto window_subsystem_st::finalize() -> void {
    information(std::format("finalizing {0}", m_name));
    std::ignore = vkfw::terminate();
    information(std::format("{0} finalized", m_name));
    m_initialized = false;
  }
}