#include "mwc/core/diagnostic/log/subsystem.hpp"

#include <contracts>

import mwc_logging_subsystem_switch;

import std;

namespace {
  constexpr auto assertion_kind_string(std::contracts::assertion_kind a_assertion_kind) {
    using enum std::contracts::assertion_kind;
    switch (a_assertion_kind) {
      case pre : return "precondition";
      case post : return "postcondition";
      case assert : return "assertion";
      default : contract_assert(false); std::unreachable();
    }
  }
  constexpr auto evaluation_semantic_string(std::contracts::evaluation_semantic a_evaluation_semantic) {
    using enum std::contracts::evaluation_semantic;
    switch (a_evaluation_semantic) {
      case enforce : return "enforce";
      case observe : return "observe";
      default : contract_assert(false); std::unreachable();
    }
  }
  constexpr auto detection_mode_string(std::contracts::detection_mode a_detection_mode) {
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
  const auto detection_mode = detection_mode_string(a_violation.detection_mode());
  if constexpr (mwc::diagnostic::logging_subsystem_switch()) {
    mwc::critical(std::format("contract violation: {0}\nassertion kind: {1}\nevaluation semantic: {2}\ndetection mode: {3}\n",
                              a_violation.comment(),
                              assertion_kind,
                              evaluation_semantic,
                              detection_mode),
                  a_violation.location());

    mwc::diagnostic::log::global::logging_subsystem.finalize();
  }
}
