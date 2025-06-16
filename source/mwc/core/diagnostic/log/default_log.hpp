#pragma once

#include "mwc/core/diagnostic/log/log.hpp"

import mwc_default_logging;
import mwc_event_severity;
import mwc_file_type;
import mwc_output_stream;
import mwc_contract_assertion;
import mwc_empty_type;
import mwc_subsystem;

import std;

namespace mwc {
  namespace diagnostic {
    namespace log {
      struct log_subsystem_st : public mwc::subsystem_st {
        using mwc::subsystem_st::subsystem_st;

        auto initialize() -> void override final;
        auto finalize() -> void override final;
      };

      // default log type:
      // if default logging is enabled -> log_ct
      // if default logging is not enabled -> optimized out empty_st
      using default_log_t = std::conditional_t<diagnostic::default_logging(), log_ct, empty_st>;

      auto initialize_default_log() -> default_log_t post(r : contract::validate_storage(r.storage()));
      inline default_log_t s_default_log = default_logging() ? initialize_default_log() : default_log_t {};
      template <bool tp_default_logging = default_logging()>
      auto finalize_default_log() -> void
        requires(tp_default_logging)
      pre(contract::validate_storage(s_default_log.storage()));
    }
  }
  // globally available logging functions
  // enabled by the compile time default_logging() switch
  template <bool tp_default_logging = diagnostic::default_logging()>
  auto information(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_default_logging)
  pre(contract::validate_storage(a_message));
  template <bool tp_default_logging = diagnostic::default_logging()>
  auto warning(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_default_logging)
  pre(contract::validate_storage(a_message));
  template <bool tp_default_logging = diagnostic::default_logging()>
  auto error(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_default_logging)
  pre(contract::validate_storage(a_message));
  template <bool tp_default_logging = diagnostic::default_logging()>
  auto critical(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_default_logging)
  pre(contract::validate_storage(a_message));

  inline auto s_log_subsystem = diagnostic::log::log_subsystem_st {{&s_root_subsystem}, string_view_t {"log subsystem"}};
}