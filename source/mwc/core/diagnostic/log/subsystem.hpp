#pragma once

#include "mwc/core/contract/definition.hpp"

#define SUB "\n\t↳"

#include "mwc/core/chrono/subsystem.hpp"
#include "mwc/core/diagnostic/log/log.hpp"
#include "mwc/core/filesystem/file_type.hpp"

import mwc_logging_subsystem_switch;
import mwc_event_severity;
import mwc_output_stream;
import mwc_contract_assertion;
import mwc_empty_type;
import mwc_chrono;
import mwc_subsystem;

import std;

namespace mwc {
  namespace diagnostic {
    namespace log {
      struct log_subsystem_st : public subsystem_st {
        using log_t = std::conditional_t<logging_subsystem_switch(), log_ct, empty_st>;
        using string_sink_t = std::conditional_t<logging_subsystem_switch(), string_t, empty_st>;

        using subsystem_st::subsystem_st;

        auto initialize() -> void override final;
        auto finalize() -> void override final;

        static inline auto string_sink = string_sink_t {};
        static inline auto log = log_t {};
      };

      namespace global {
        inline auto logging_subsystem = log_subsystem_st {{&chrono::global::chrono_subsystem}, string_view_t {"log subsystem"}};
      }
    }
  }
  // globally available logging functions
  // enabled by the compile time logging_subsystem_switch() switch
  template <bool_t tp_logging_subsystem = diagnostic::logging_subsystem_switch()>
  auto information(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_logging_subsystem)
  pre(contract::validate_storage(a_message));
  template <bool_t tp_logging_subsystem = diagnostic::logging_subsystem_switch()>
  auto warning(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_logging_subsystem)
  pre(contract::validate_storage(a_message));
  template <bool_t tp_logging_subsystem = diagnostic::logging_subsystem_switch()>
  auto error(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_logging_subsystem)
  pre(contract::validate_storage(a_message));
  template <bool_t tp_logging_subsystem = diagnostic::logging_subsystem_switch()>
  auto critical(const string_view_t a_message, const std::source_location& a_source_location = std::source_location::current())
    -> void
    requires(tp_logging_subsystem)
  pre(contract::validate_storage(a_message));
}