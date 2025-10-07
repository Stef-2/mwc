#pragma once

#include "mwc/core/contract/definition.hpp"

import mwc_definition;
import mwc_resolution;
import mwc_contract_assertion;
import mwc_project_name_string;
import mwc_type_mobility;

import std;
import vulkan_hpp;
import vkfw;

namespace mwc {
  class window_ct : public irreproducible_st {
    public:
    struct configuration_st {
      static constexpr auto default_configuration() -> const configuration_st;
      resolution_st m_resolution;
      string_view_t m_title;
      vkfw::WindowHints m_window_hints;
      bool_t m_full_screen;
    };

    window_ct(const configuration_st& a_configuration = configuration_st::default_configuration())
      pre(a_configuration.m_resolution.m_width != 0 and a_configuration.m_resolution.m_height != 0)
        pre(contract::validate_storage(a_configuration.m_title));

    auto title() const -> string_view_t;
    auto title(const string_view_t a_title) -> void pre(contract::validate_storage(a_title));
    auto resolution() const -> resolution_st;
    auto aspect_ratio() const -> resolution_st::aspect_ratio_t;
    auto full_screen() const -> bool_t;
    auto full_screen(bool_t a_full_screen) -> void;
    auto closing_requested() const -> bool_t;
    auto request_closing() const -> void;
    auto vkfw_monitor() const -> const vkfw::Monitor&;
    auto vkfw_window() const -> const vkfw::UniqueWindow&;
    template <typename tp_this>
    [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

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
  auto window_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
    return std::forward_like<decltype(a_this)>(a_this.m_configuration);
  }
}