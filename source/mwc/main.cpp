#include "mwc/mwc.hpp"
#include "mwc/core/filesystem/subsystem.hpp"
#include "mwc/ecs/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"
#include "mwc/input/subsystem.hpp"
#include "mwc/core/filesystem/subsystem.hpp"

import mwc_subsystem;
//import mwc_ecs_definition;
//import mwc_ecs_component;
import mwc_ctti;
import mwc_vertex_model;

import std;

import vkfw;

int main() {
  const auto scene = mwc::input::read_scene_file("/home/billy/dev/mwc/data/mesh/cube.glb");
  for (auto i = 0; i < scene.m_meshes.back().m_vertex_storage.size(); i += 3 * 4) {
    const auto vertex = std::bit_cast<mwc::geometry::vertex_position_st*>(scene.m_meshes.back().m_vertex_storage.data() + i);
    std::cout << vertex->m_position.x() << " " << vertex->m_position.y() << " " << vertex->m_position.z() << '\n';
  }
  const auto e = mwc::ecs::generate_entity<test2, test0, test1>(test2 {.c = 'b'}, test0 {.i = 69}, test1 {.f = 6.4f});
  const auto c = mwc::ecs::entity_components<test2, test0, test1>(e);
  const auto ac = mwc::ecs::ecs_subsystem_st::entity_archetype_map[1].m_archetype->component_data_row(0);
  test0* i = (test0*)ac[0].data();
  test1* f = (test1*)ac[1].data();
  test2* cc = (test2*)ac[2].data();
  mwc::ecs::insert_components<test3>(1, test3 {.b = true});
  mwc::ecs::remove_components<test1>(1);
  auto after_ac = mwc::ecs::entity_components<test2, test0, test3>(e);
  const auto ac2 = mwc::ecs::ecs_subsystem_st::entity_archetype_map[1].m_archetype->component_data_row(0);

  std::cout << "hash arch sz: " << mwc::ecs::ecs_subsystem_st::hash_archetype_map.size() << '\n';
  std::cout << "ent arch sz: " << mwc::ecs::ecs_subsystem_st::entity_archetype_map.size() << '\n';
  std::cout << "comp arch sz: " << mwc::ecs::ecs_subsystem_st::component_archetype_map.size() << '\n';
  for (auto& [k, v] : mwc::ecs::ecs_subsystem_st::component_archetype_map) {
    std::cout << "comp GUID: " << k << '\n';
    for (auto& [k2, v2] : v)
      std::cout << "arch id: " << k2->m_index << "arch comp id: " << v2 << '\n';
  }
  mwc::ecs::destroy_entity(e);
  //mwc::ecs::entity_insert_components<test2>(e, {.c = 'x'});
  constexpr auto x = test0::identity;
  constexpr auto y = test1::identity;
  std::cout << x << " " << y << std::endl;

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
