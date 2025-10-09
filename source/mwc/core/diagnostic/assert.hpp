#pragma once

#include "mwc/core/diagnostic/log/logging.hpp"

import mwc_logging_subsystem_switch;

import std;

namespace mwc {
  inline auto sanity_check(const auto& a_expression, const std::source_location& a_location = std::source_location::current())
    -> void {
    if (not a_expression) {
      if constexpr (diagnostic::logging_subsystem_switch()) {
        mwc::critical("assertion failed", a_location);
      } else {
        std::abort();
      }
    }
  }
}