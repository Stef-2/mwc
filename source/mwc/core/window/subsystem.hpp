#include "mwc/core/diagnostic/log/subsystem.hpp"

import vkfw;

import mwc_subsystem;

namespace mwc {
  struct window_subsystem_st : public subsystem_st {
    using subsystem_st::subsystem_st;

    auto initialize() -> void override final;
    auto finalize() -> void override final;
  };

  inline auto s_window_subsystem =
    window_subsystem_st {{&diagnostic::log::s_logging_subsystem}, string_view_t {"window subsystem"}};
}