#include <experimental/contract>

import std;

// gcc currently does not support combining contracts and modules as both are experimental
// the violation handler has to be defined in a regular translation unit instead of a module
// note: move this to a module unit once gcc supports both features
void handle_contract_violation(
  const std::experimental::contract_violation& a_violation) {
  std::print("contract violation HERE: {0}\n", a_violation.comment());
}