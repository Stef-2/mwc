#pragma once

import mwc_definition;
import mwc_resolution;
import mwc_contract_assertion;
import mwc_project_name_string;

import vulkan_hpp;
import vkfw;

import std;

namespace mwc {
  class window_ct {
    public:
    struct configuration_st {
      static constexpr auto default_configuration() -> const configuration_st;
      resolution_st m_resolution;
      string_view_t m_title;
      vkfw::WindowHints m_window_hints;
      bool m_full_screen;
    };

    window_ct(const configuration_st& a_configuration = configuration_st::default_configuration())
      pre(a_configuration.m_resolution.m_width != 0 and a_configuration.m_resolution.m_height != 0)
        pre(contract::validate_storage(a_configuration.m_title));
    window_ct(const window_ct&) = delete("move only type");
    auto operator=(const window_ct&) -> window_ct& = delete("move only type");

    auto title() const -> string_view_t;
    auto title(const string_view_t a_title) -> void pre(contract::validate_storage(a_title));
    auto resolution() const -> resolution_st;
    auto aspect_ratio() const -> resolution_st::aspect_ratio_t;
    auto full_screen() const -> bool;
    auto full_screen(bool a_full_screen) -> void;
    auto vkfw_monitor() const -> const vkfw::Monitor&;
    auto vkfw_window() const -> const vkfw::UniqueWindow&;
    template <typename tp_this>
    auto configuration(this tp_this&& a_this) -> configuration_st&;

    private:
    vkfw::Monitor m_monitor;
    vkfw::UniqueWindow m_window;
    configuration_st m_configuration;
  };
  // implementation
  constexpr auto window_ct::configuration_st::default_configuration() -> const configuration_st {
    return {{resolution_et::e_default_windowed},
            project_name_string(),
            {.resizable = false,
             .decorated = true,
             .focused = true,
             .autoIconify = false,
             .centerCursor = true,
             .focusOnShow = true,
             .srgbCapable = true,
             .clientAPI = vkfw::ClientAPI::eNone},
            false};
  }
  template <typename tp_this>
  auto window_ct::configuration(this tp_this&& a_this) -> configuration_st& {
    return a_this.m_configuration;
  }
}