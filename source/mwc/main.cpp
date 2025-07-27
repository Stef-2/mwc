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

int main() {
  //using ttt = decltype(mwc::ecs::test<test1, test0, test2>({.f = 23}, {.i = 2}, {.c = 'a'}));
  //static_assert(std::is_same_v<ttt, char>);
  //constexpr auto xx = mwc::ecs::component_hash<test1>();
  //static_assert(xx == 23233);
  using t = decltype(mwc::ctti::observe_type_list<mwc::ecs::component_type_list_st>())::component_at_index_t<1>;
  //static_assert(std::is_same_v<t, char>);
  //constexpr auto yy = mwc::ecs::archetype_hash<test3, test2>();
  const auto e = mwc::ecs::generate_entity<test2, test0, test1>(test2 {.c = 'b'}, test0 {.i = 69}, test1 {.f = 6.4f});
  const auto c = mwc::ecs::entity_components<test2, test0, test1>(e);
  const auto ac = mwc::ecs::ecs_subsystem_st::entity_archetype_map[1].m_archetype->component_data_row(0);
  test0* i = (test0*)ac[0].data();
  test1* f = (test1*)ac[1].data();
  test2* cc = (test2*)ac[2].data();
  mwc::ecs::destroy_entity(e);
  //mwc::ecs::entity_insert_components<test2>(e, {.c = 'x'});
  constexpr auto x = test0::identity;
  constexpr auto y = test1::identity;
  std::cout << x << " " << y << std::endl;

  //mwc::ecs::generate_archetype<test0, test1, test2>();
  /*const auto cc = mwc::ecs::ecs_subsystem_st::archetypes[0].get()->component_count();
  const auto ci = mwc::ecs::ecs_subsystem_st::archetypes[0].get()->component_indices();*/
  //mwc::ecs::destroy_archetype<test0, test1, test2>();
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
