module;

#include <experimental/contract>

export module mwc_contract_violation;

import mwc_breakpoint;

import std;

export auto handle_contract_violation(
  const std::experimental::contract_violation& a_violation) {

  mwc::breakpoint();
  std::cout << "omfg!!!";
  //std::is_debugger_present();
  std::abort();
}