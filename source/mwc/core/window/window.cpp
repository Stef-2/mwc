#include "mwc/core/window/window.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace mwc {
  window_ct::window_ct(const configuration_st& a_configuration)
  : m_monitor {std::invoke([] {
      const auto primary_monitor = vkfw::getPrimaryMonitor();
      contract_assert(vkfw::check(primary_monitor.result));
      const auto [physical_width, physical_height] = primary_monitor.value.getPhysicalSize().value;
      const auto [work_area_offset_x, work_area_offset_y, work_area_size_x, work_area_size_y] =
        primary_monitor.value.getWorkarea().value;
      const auto name = primary_monitor.value.getName().value;
      information(std::format(
        "selecting primary monitor: {0}\nphysical size: {1}mm x {2}mm\nwork area: offset {3}x{4} - size {5}x{6}", name,
        physical_width, physical_height, work_area_offset_x, work_area_offset_y, work_area_size_x, work_area_size_y));
      return vkfw::getPrimaryMonitor().value;
    })},
    m_window {std::invoke([&a_configuration, this] {
      const auto extent = a_configuration.m_full_screen ? "fullscreen" : "non fullscreen";
      information(std::format("creating {0} window with resolution {1}x{2}", extent, a_configuration.m_resolution.m_width,
                              a_configuration.m_resolution.m_height));
      return vkfw::createWindowUnique(a_configuration.m_resolution.m_width, a_configuration.m_resolution.m_height,
                                      a_configuration.m_title.data(), a_configuration.m_window_hints,
                                      a_configuration.m_full_screen ? m_monitor : nullptr)
        .value;
    })},
    m_configuration {a_configuration} {}
}