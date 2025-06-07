#include <__ostream/basic_ostream.h>
#include <contracts>
#include <iostream>

import std;

void handle_contract_violation(const std::contracts::contract_violation& a_violation) {
  std::cerr << "contract got violated again:\n" << a_violation.comment() << '\n' << "file: " << a_violation.location().file_name() << '\n' << "function: " << a_violation.location().function_name() << '\n';
}
