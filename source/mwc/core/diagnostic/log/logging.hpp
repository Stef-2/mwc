#pragma once

#define SUB "\n\t↳"

#include "mwc/core/diagnostic/log/subsystem.hpp"

import mwc_definition;
import mwc_contract_assertion;
import mwc_logging_subsystem_switch;

import std;

namespace mwc {
  // globally available logging functions
  // enabled by the compile time logging_subsystem_switch() switch
  template <bool_t tp_logging_subsystem = diagnostic::logging_subsystem_switch()>
  auto information(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_logging_subsystem) {
    diagnostic::log::global::logging_subsystem.log.information(a_message, a_source_location);
  }
  template <bool_t tp_logging_subsystem = diagnostic::logging_subsystem_switch()>
  auto warning(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_logging_subsystem) {
    diagnostic::log::log_subsystem_st::log.warning(a_message, a_source_location);
  }
  template <bool_t tp_logging_subsystem = diagnostic::logging_subsystem_switch()>
  auto error(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_logging_subsystem) {
    diagnostic::log::log_subsystem_st::log.error(a_message, a_source_location);
  }
  template <bool_t tp_logging_subsystem = diagnostic::logging_subsystem_switch()>
  auto critical(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_logging_subsystem) {
    diagnostic::log::log_subsystem_st::log.critical(a_message, a_source_location);
    std::abort();
  }
}