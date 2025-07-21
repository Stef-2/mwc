#include "mwc/mwc.hpp"
#include "mwc/core/filesystem/subsystem.hpp"
#include "mwc/ecs/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"
import mwc_subsystem;
import mwc_ecs_definition;

import std;

import vkfw;

struct test2 : mwc::ecs::component_t<test2> {
  int i;
};
struct test3 : mwc::ecs::component_t<test3> {
  float f;
};

int main() {
  const auto e = mwc::ecs::generate_entity<test2, test3>();
  constexpr auto x = test2::identity;
  constexpr auto y = test3::identity;
  std::cout << x << " " << y << std::endl;

  mwc::ecs::archetype_ct<2> arch;
  std::cout << "cap: " << arch.m_components[1].m_index << '\n';

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
