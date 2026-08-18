#include "mwc/core/diagnostic/log/logging.hpp"

#undef assert

import mwc_logging_subsystem_switch;
import mwc_log_subsystem;

import std;

namespace {
  constexpr auto assertion_kind_string(const std::contracts::assertion_kind a_assertion_kind) {
    using enum std::contracts::assertion_kind;
    switch (a_assertion_kind) {
      case pre : return "precondition";
      case post : return "postcondition";
      case assert : return "assertion";
      default : contract_assert(false); std::unreachable();
    }
  }
  constexpr auto evaluation_semantic_string(const std::contracts::evaluation_semantic a_evaluation_semantic) {
    using enum std::contracts::evaluation_semantic;
    switch (a_evaluation_semantic) {
      case enforce : return "enforce";
      case observe : return "observe";
      default : contract_assert(false); std::unreachable();
    }
  }
  constexpr auto detection_mode_string(const std::contracts::detection_mode a_detection_mode) {
    using enum std::contracts::detection_mode;
    switch (a_detection_mode) {
      case predicate_false : return "predicate false";
      case evaluation_exception : return "evaluation exception";
      default : contract_assert(false); std::unreachable();
    }
  }
}
void handle_contract_violation(const std::contracts::contract_violation& a_violation) {
  const auto assertion_kind = assertion_kind_string(a_violation.kind());
  const auto evaluation_semantic = evaluation_semantic_string(a_violation.semantic());
  const auto detection_mode = detection_mode_string(a_violation.mode());
  const auto terminating = a_violation.is_terminating();
  const auto contract_violation_string
    = std::format("contract violation: {0}" SUB "assertion kind: {1}" SUB "evaluation semantic: {2}" SUB
                  "detection mode: {3}" SUB "terminating: {4}",
                  a_violation.comment(),
                  assertion_kind,
                  evaluation_semantic,
                  detection_mode,
                  terminating);
  if constexpr (mwc::diagnostic::logging_subsystem_switch()) {
    if (terminating) {
      mwc::critical(contract_violation_string, a_violation.location());
      mwc::diagnostic::log::global::logging_subsystem.finalize();
    } else {
      mwc::error(contract_violation_string, a_violation.location());
    }
  } else {
    std::format_to(std::back_inserter(contract_violation_string),
                   SUB "source location: {0}" SUB "stacktrace: {1}",
                   a_violation.location(),
                   std::stacktrace::current(/*skip*/ 1));
    std::println("{0}", contract_violation_string);
  }
}