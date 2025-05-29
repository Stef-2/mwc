//#pragma once

// gcc currently implements c++ contract syntax according to c++20 working paper N4820
// with proposed enhancements from papers P1290, P1332, and P1429

// the syntax accepted into the c++26 is the one proposed in P2961
// these macros emulate the natural syntax proposed in this paper

/*#define pre(a_predicate) [[pre:a_predicate]]
#define post(a_predicate) [[post a_predicate]]
#define contract_assert(a_predicate) [[assert:a_predicate]]*/