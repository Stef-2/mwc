module;

#ifndef __clang__
  #include <experimental/contract>
#endif

export module mwc_contract_violation;

import mwc_breakpoint;

import std;

export auto handle_contract_violation(
  const std::experimental::contract_violation& a_violation) {

  mwc::breakpoint();
  std::cout << "omfg!!!";
}