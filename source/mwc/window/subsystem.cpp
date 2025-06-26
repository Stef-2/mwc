#include "mwc/window/subsystem.hpp"

namespace mwc {
  auto window_subsystem_st::initialize() -> void {
    const auto vkfw_initialization = vkfw::init();
    contract_assert(vkfw::check(vkfw_initialization));
    const auto vulkan_support = vkfw::vulkanSupported();
    contract_assert(vulkan_support);
    m_initialized = true;
  }
  auto window_subsystem_st::finalize() -> void {
    std::ignore = vkfw::terminate();
    m_initialized = false;
  }
}