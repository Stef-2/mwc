#pragma once

#include "mwc/core/diagnostic/log/log.hpp"

import mwc_default_logging;
import mwc_event_severity;
import mwc_file_type;
import mwc_output_stream;
import mwc_contract_assertion;
import mwc_empty_type;

import std;

namespace mwc {
  namespace diagnostic {
    namespace log {
      // default log type:
      // if default logging is enabled -> log_ct
      // if default logging is not enabled -> optimized out empty_st
      using default_log_t = std::conditional_t<diagnostic::default_logging(), log_ct, empty_st>;

      auto initialize_default_log() -> default_log_t post(r : contract::validate_storage(r.storage()));
      const inline default_log_t s_default_log = default_logging() ? initialize_default_log() : default_log_t {};
      auto finalize_default_log() -> void pre(contract::validate_storage(s_default_log.storage()));
    }
  }
  auto information(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void pre(contract::validate_storage(a_message));
  auto warning(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void pre(contract::validate_storage(a_message));
  auto error(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void pre(contract::validate_storage(a_message));
  auto critical(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void pre(contract::validate_storage(a_message));
}