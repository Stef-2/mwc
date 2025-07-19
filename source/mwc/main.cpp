#include "mwc/mwc.hpp"
#include "mwc/core/filesystem/subsystem.hpp"

import mwc_subsystem;
import mwc_consteval_counter;

import std;

import vkfw;

template <typename tp>
struct test {
  static constexpr auto id = mwc::ExtractThenUpdateCurrentState<0>();
};

struct test2 : test<test2> {};
struct test3 : test<test3> {};

int main() {
  constexpr auto x = test2::id;
  constexpr auto y = test3::id;
  std::cout << x << " " << y << std::endl;
  return 0;
  mwc::initialize_subsystems();
  mwc::mwc_ct mwc {};

  while (true) {
    vkfw::pollEvents();
    mwc.m_graphics.render();
  }

  mwc::finalize_subsystems();

  return 0;
}
