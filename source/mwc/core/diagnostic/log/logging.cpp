#include "mwc/core/diagnostic/log/logging.hpp"

import mwc_log_subsystem;
import mwc_contract_assertion;

namespace mwc {
  template <>
  auto information<true>(const string_view_t a_message, const std::source_location& a_source_location) -> void {
    diagnostic::log::global::logging_subsystem.log.information(a_message, a_source_location);
  }
  template <>
  auto warning<true>(const string_view_t a_message, const std::source_location& a_source_location) -> void {
    diagnostic::log::log_subsystem_st::log.warning(a_message, a_source_location);
  }
  template <>
  auto error<true>(const string_view_t a_message, const std::source_location& a_source_location) -> void {
    diagnostic::log::log_subsystem_st::log.error(a_message, a_source_location);
  }
  template <>
  auto critical<true>(const string_view_t a_message, const std::source_location& a_source_location) -> void {
    diagnostic::log::log_subsystem_st::log.critical(a_message, a_source_location);
    std::abort();
  }
}