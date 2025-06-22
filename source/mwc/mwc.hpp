#pragma once

#include "mwc/core/utility/version.hpp"
#include "mwc/window/window.hpp"

namespace mwc {
  class mwc_ct {
    public:
    struct configuration_st {
      static constexpr auto default_configuration() -> const configuration_st;

      semantic_version m_version;
      window_ct::configuration_st m_window_configuration;
    };
    mwc_ct(const configuration_st& a_configuration = configuration_st::default_configuration());
    mwc_ct(const mwc_ct&) = delete;
    auto operator=(const mwc_ct&) -> mwc_ct& = delete;
    mwc_ct(const mwc_ct&&) = delete;
    auto operator=(const mwc_ct&&) noexcept -> mwc_ct& = delete;
    ~mwc_ct();

    private:
    window_ct m_window;
    configuration_st m_configuration;
  };

  // implementation
  constexpr auto mwc_ct::configuration_st::default_configuration() -> const configuration_st {
    return configuration_st {{0, 3, 2}, window_ct::configuration_st::default_configuration()};
  }
}