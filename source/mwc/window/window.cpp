#include "mwc/window/window.hpp"
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
        "selecting primary monitor: {0}\nphysical size: {1}mm x {2}mm\nwork area: offset {3}px x {4}px - size {5}px x {6}px",
        name, physical_width, physical_height, work_area_offset_x, work_area_offset_y, work_area_size_x, work_area_size_y));
      return vkfw::getPrimaryMonitor().value;
    })},
    m_window {std::invoke([&a_configuration, this] {
      const auto extent = a_configuration.m_full_screen ? "fullscreen" : "non fullscreen";
      information(std::format("creating {0} window with resolution {1}px x{2}px", extent, a_configuration.m_resolution.m_width,
                              a_configuration.m_resolution.m_height));
      return vkfw::createWindowUnique(a_configuration.m_resolution.m_width, a_configuration.m_resolution.m_height,
                                      a_configuration.m_title.data(), a_configuration.m_window_hints,
                                      a_configuration.m_full_screen ? m_monitor : nullptr)
        .value;
    })},
    m_configuration {a_configuration} {}
  auto window_ct::title() const -> string_view_t {
    return m_configuration.m_title;
  }
  auto window_ct::title(const string_view_t a_title) -> void {
    m_configuration.m_title = a_title;
    const auto result = m_window->setTitle(a_title);

    contract_assert(vkfw::check(result));
  }
  auto window_ct::resolution() const -> resolution_st {
    return m_configuration.m_resolution;
  }
  auto window_ct::aspect_ratio() const -> resolution_st::aspect_ratio_t {
    return m_configuration.m_resolution.aspect_ratio();
  }
  auto window_ct::full_screen() const -> bool {
    return m_configuration.m_full_screen;
  }
  auto window_ct::full_screen(bool a_full_screen) -> void {
    if (m_configuration.m_full_screen != a_full_screen) {
      const auto [work_area_offset_x, work_area_offset_y, work_area_size_x, work_area_size_y] = m_monitor.getWorkarea().value;
      const auto result = m_window->setMonitor(a_full_screen ? m_monitor : nullptr, work_area_offset_x, work_area_offset_y,
                                               work_area_size_x, work_area_size_y);

      contract_assert(vkfw::check(result));
    }

    m_configuration.m_full_screen = a_full_screen;
  }
  auto window_ct::vkfw_monitor() const -> const vkfw::Monitor& {
    return m_monitor;
  }
  auto window_ct::vkfw_window() const -> const vkfw::UniqueWindow& {
    return m_window;
  }
}