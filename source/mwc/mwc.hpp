#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/utility/semantic_version.hpp"
#include "mwc/graphics/graphics.hpp"
#include "mwc/window/window.hpp"

import mwc_subsystem;

namespace mwc {
  class mwc_ct {
    public:
    struct configuration_st {
      static constexpr auto default_configuration() -> const configuration_st;

      semantic_version_st m_version;
      window_ct::configuration_st m_window_configuration;
    };
    mwc_ct(const configuration_st& a_configuration = configuration_st::default_configuration());
    mwc_ct(const mwc_ct&) = delete;
    auto operator=(const mwc_ct&) -> mwc_ct& = delete;
    mwc_ct(const mwc_ct&&) = delete;
    auto operator=(const mwc_ct&&) noexcept -> mwc_ct& = delete;
    ~mwc_ct();

    // private :
    window_ct m_window;
    graphics::graphics_ct m_graphics;
    configuration_st m_configuration;
  };

  // implementation
  constexpr auto mwc_ct::configuration_st::default_configuration() -> const configuration_st {
    return configuration_st {.m_version = semantic_version_st {0, 3, 3},
                             .m_window_configuration = window_ct::configuration_st::default_configuration()};
  }
}