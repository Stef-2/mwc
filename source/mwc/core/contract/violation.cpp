//#include <experimental/contract>
//#include <print>
//import std;
//#include <iostream>

// gcc currently does not support combining contracts and modules as both are experimental
// the violation handler has to be defined in a regular translation unit instead of a module
// note: move this to a module unit once gcc fully supports both features
/*namespace std::experimental {
  struct contract_violation;
}*/
/*void handle_contract_violation(
  const std::experimental::contract_violation& a_violation) {*/
//std::cout << "contract violation HERE: {0}\n" << a_violation.comment();
//std::terminate();
/*}*/