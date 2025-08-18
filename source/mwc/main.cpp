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
  /*const auto scene = mwc::input::read_scene_file("/home/billy/dev/mwc/data/mesh/cube.glb");
  std::cout << scene.m_meshes.back().m_vertex_storage.size() / scene.m_meshes.back().m_vertex_model_size << '\n';
  for (auto i = 0; i < scene.m_meshes.back().m_vertex_storage.size(); i += scene.m_meshes.back().m_vertex_model_size) {
    auto pos = std::bit_cast<mwc::array_t<float, 3>*>(scene.m_meshes.back().m_vertex_storage.data() + i);
    auto nor = std::bit_cast<mwc::array_t<float, 3>*>(scene.m_meshes.back().m_vertex_storage.data() + i + 12);
    auto tan = std::bit_cast<mwc::array_t<float, 4>*>(scene.m_meshes.back().m_vertex_storage.data() + i + 12 + 12);
    auto uv = std::bit_cast<mwc::array_t<float, 2>*>(scene.m_meshes.back().m_vertex_storage.data() + i + 12 + 12 + 16);

    std::cout << "P: (" << (*pos)[0] << " " << (*pos)[1] << " " << (*pos)[2] << ')';
    std::cout << " N: (" << (*nor)[0] << " " << (*nor)[1] << " " << (*nor)[2] << ')';
    std::cout << " T: (" << (*tan)[0] << " " << (*tan)[1] << " " << (*tan)[2] << " " << (*tan)[3] << ')';
    std::cout << " U: (" << (*uv)[0] << " " << (*uv)[1] << ')' << "\n";
  }
  std::cout << "idx count: " << scene.m_meshes.back().m_index_storage.size() << '\n';
  for (const auto idx : scene.m_meshes.back().m_index_storage)
    std::cout << idx << "\n";
  */
  const auto e = mwc::ecs::generate_entity<test2, test0, test1>(test2 {.c = 'b'}, test0 {.i = 69}, test1 {.f = 6.4f});
  const auto e2 = mwc::ecs::generate_entity<test2, test0, test1>(test2 {.c = 'b'}, test0 {.i = 69}, test1 {.f = 6.4f});
  //const auto c = mwc::ecs::entity_components<test2, test0, test1>(e);
  const auto ac = mwc::ecs::ecs_subsystem_st::entity_archetype_map[1].m_archetype->component_data_row(0);
  //test0* i = (test0*)ac[0].data();
  //test1* f = (test1*)ac[1].data();
  //test2* cc = (test2*)ac[2].data();
  //mwc::ecs::insert_components<test3>(1, test3 {.b = true});
  //mwc::ecs::remove_components<test1>(1);
  //auto after_ac = mwc::ecs::entity_components<test2, test0, test3>(e);
  //const auto ac2 = mwc::ecs::ecs_subsystem_st::entity_archetype_map[1].m_archetype->component_data_row(0);

  /*
  std::cout << "hash arch sz: " << mwc::ecs::ecs_subsystem_st::hash_archetype_map.size() << '\n';
  std::cout << "ent arch sz: " << mwc::ecs::ecs_subsystem_st::entity_archetype_map.size() << '\n';
  std::cout << "comp arch sz: " << mwc::ecs::ecs_subsystem_st::component_archetype_map.size() << '\n';
  for (auto& [k, v] : mwc::ecs::ecs_subsystem_st::component_archetype_map) {
    std::cout << "comp GUID: " << k << '\n';
    for (auto& [k2, v2] : v)
      std::cout << "arch id: " << k2->m_index << "arch comp id: " << v2 << '\n';
  }*/
  //mwc::ecs::destroy_entity(e);
  //mwc::ecs::entity_insert_components<test2>(e, {.c = 'x'});
  constexpr auto x = test0::index;
  constexpr auto y = test1::index;
  std::cout << x << " " << y << std::endl;

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