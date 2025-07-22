#include "mwc/mwc.hpp"
#include "mwc/core/filesystem/subsystem.hpp"
#include "mwc/ecs/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"
import mwc_subsystem;
import mwc_ecs_definition;
import mwc_ecs_component;
import mwc_ctti;

import std;

import vkfw;

struct test0 : mwc::ecs::component_st<test0> {
  int i;
};
struct test1 : mwc::ecs::component_st<test1> {
  float f;
};
struct test2 : mwc::ecs::component_st<test2> {
  char c;
};
struct test3 : mwc::ecs::component_st<test3> {
  char c;
};

int main() {
  constexpr auto xx = mwc::ecs::archetype_hash<test3, test1>();
  //static_assert(xx == 23233);
  //constexpr auto yy = mwc::ecs::archetype_hash<test3, test2>();
  const auto e = mwc::ecs::generate_entity<test0, test1>({.i = 33}, {.f = 23.4f});
  constexpr auto x = test0::identity;
  constexpr auto y = test1::identity;
  std::cout << x << " " << y << std::endl;

  mwc::ecs::generate_archetype<test0, test2>();
  //std::cout << "cap: " << arch.m_components[1].m_index << '\n';

  return 0;

  // testing =======================
  mwc::initialize_subsystems();
  mwc::mwc_ct mwc {};

  while (true) {
    std::ignore = vkfw::pollEvents();
    mwc.m_graphics.render();
  }

  mwc::finalize_subsystems();

  return 0;
}
