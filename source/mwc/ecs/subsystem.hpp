#pragma once

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_ecs_definition;
import mwc_ecs_component;
import mwc_observer_ptr;

struct test0 : mwc::ecs::component_st<test0> {
  int i = 23;
};

struct test1 : mwc::ecs::component_st<test1> {
  float f = 2.0f;
};
struct test2 : mwc::ecs::component_st<test2> {
  char c = 'a';
};
struct test3 : mwc::ecs::component_st<test3> {
  bool b = true;
};

namespace mwc {
  namespace ecs {
    static constexpr auto invalid_entity = entity_index_t {0};
    static constexpr auto invalid_archetype = archetype_index_t {0};
    struct ecs_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline auto entity_index = entity_index_t {1};
      static inline auto archetype_index = archetype_index_t {1};

      static inline auto hash_archetype_map = unordered_map_t<component_hash_t, archetype_st> {};
      static inline auto entity_archetype_map = unordered_map_t<entity_index_t, archetype_entity_index_st> {};
      static inline auto component_archetype_map = unordered_multimap_t<component_index_t, archetype_component_index_st> {};
    };
    /*
    template <component_c... tp_components>
    auto generate_archetype() -> void {
      using t = decltype(sort_components<tuple_t<>, tp_components...>());
      static_assert(std::is_same_v<t, char>);
      ecs_subsystem_st::archetypes.emplace_back(std::make_unique<archetype_ct<tp_components...>>());
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto destroy_archetype() -> void {
      constexpr auto hash = archetype_hash<tp_components...>();
      for (auto i = 0uz; i < ecs_subsystem_st::archetypes.size(); ++i)
        if (ecs_subsystem_st::archetypes[i].get()->hash() == hash)
          ecs_subsystem_st::archetypes.erase(ecs_subsystem_st::archetypes.begin() + i);
    }*/

    template <component_c... tp_components>
    [[nodiscard]] constexpr auto generate_entity(tp_components&&... a_components) -> entity_index_t
    /*post(r : r != std::numeric_limits<entity_t>::max())*/ {
      // determine if the components are sorted according to their identification
      if constexpr (not std::is_same_v<tuple_t<tp_components...>, decltype(sorted_component_types<tp_components...>())>) {
        auto sorted_component_tuple = sorted_component_types<tp_components...>();
        ((std::get<tp_components>(sorted_component_tuple) = a_components), ...);
        auto [... sorted_components] = sorted_component_tuple;
        return generate_entity<decltype(sorted_components)...>(std::forward<decltype(sorted_components)>(sorted_components)...);
      } else {
        constexpr auto hash = component_hash<tp_components...>();
        const auto entity_index = ecs_subsystem_st::entity_index;
        ++ecs_subsystem_st::entity_index;

        // determine if a matching archetype already exists
        auto archetype_iterator = ecs_subsystem_st::hash_archetype_map.find(hash);
        if (archetype_iterator != ecs_subsystem_st::hash_archetype_map.end()) {
          // insert component data into the archetype
          archetype_iterator->second.insert_component_row(std::forward<tp_components>(a_components)...);
          // register entity - archetype mapping
          ecs_subsystem_st::entity_archetype_map.emplace(
            entity_index,
            archetype_entity_index_st {&archetype_iterator->second, archetype_iterator->second.m_element_count - 1});
          // a matching archetype does not exist, create one
        } else {
          const auto archetype_index = ecs_subsystem_st::archetype_index;
          ++ecs_subsystem_st::archetype_index;

          archetype_iterator = ecs_subsystem_st::hash_archetype_map
                                 .emplace(hash, archetype_st {archetype_index, std::forward<tp_components>(a_components)...})
                                 .first;
          // register entity - archetype mapping
          ecs_subsystem_st::entity_archetype_map.emplace(
            entity_index,
            archetype_entity_index_st {&archetype_iterator->second, archetype_iterator->second.m_element_count - 1});
          // register component - archetype mapping
          const auto lambda = [&archetype_iterator,
                               &archetype_index]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
            ((ecs_subsystem_st::component_archetype_map.emplace(
               tp_components... [tp_index] ::identity,
               archetype_component_index_st { .m_archetype = &archetype_iterator->second, .m_component_index = tp_index })),
             ...);
          };
          lambda(std::make_index_sequence<sizeof...(tp_components)> {});
        }
        return entity_index;
      }
    }
    inline auto destroy_entity(const entity_index_t a_entity)
      -> void /*pre(ecs_subsystem_st::entity_archetype_map.contains(a_entity))*/ {
      const auto archetype = ecs_subsystem_st::entity_archetype_map[a_entity];
      archetype.m_archetype->remove_component_row(archetype.m_entity_index);
      ecs_subsystem_st::entity_archetype_map.erase(a_entity);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    inline auto entity_components(const entity_index_t m_entity_index) {
      // determine the matching archetype
      const auto archetype = ecs_subsystem_st::entity_archetype_map[m_entity_index];
      contract_assert(archetype.m_archetype != nullptr and m_entity_index != invalid_entity);

      // components are returned in the requested order, not the internally sorted one
      auto components = tuple_t<tp_components...> {};
      std::apply(
        [&archetype](auto&... a_components) -> void {
          ((a_components = std::bit_cast<vector_t<std::remove_cvref_t<decltype(a_components)>>*>(
                             archetype.m_archetype
                               ->m_component_data[ecs_subsystem_st::component_archetype_map
                                                    .find(std::remove_cvref_t<decltype(a_components)>::identity)
                                                    ->second.m_component_index]
                               .m_data)
                             ->operator[](archetype.m_entity_index)),
           ...);
        },
        components);

      return components;
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    inline auto insert_components(const entity_index_t a_entity_index, tp_components&&... a_components) {
      // determine the current archetype
      const auto current_archetype = ecs_subsystem_st::entity_archetype_map[a_entity_index];
      contract_assert(current_archetype.m_archetype != nullptr and a_entity_index != invalid_entity);

      constexpr auto inserted_components_hash = component_hash<tp_components...>();
      // determine if this class of archetype modification has been recorded before
      if (current_archetype.m_archetype->m_modification_map.contains(inserted_components_hash)) {
        const auto target_archetype = current_archetype.m_archetype->m_modification_map[inserted_components_hash];
        target_archetype->insert_component_column();
      }
    }
    /*
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto entity_insert_components(const entity_t a_entity, tp_components&&... a_components) -> void {
      constexpr auto inserted_archetype_hash = archetype_hash<tp_components...>();

      // determing the entities current archetype, combine its hash with the new one
      for (const auto& current_archetype : ecs_subsystem_st::archetypes) {
        auto& entities = current_archetype.get()->m_entities;
        const auto entity_iterator = std::ranges::find(entities, a_entity);
        if (entity_iterator != entities.end()) {
          auto combined_hash = current_archetype.get()->hash();

          // rehash using the new component types
          const auto lambda = [&combined_hash]<component_c tp>() {
            // convert the component type identities to a string format, suitable for hashing
            constexpr auto identity_string = std::bit_cast<array_t<char, sizeof(tp::identity)>>(tp::identity);
            combined_hash += polynomial_rolling_hash(string_view_t {identity_string.data(), identity_string.size()});
          };
          (lambda.template operator()<tp_components>(), ...);

          // attempt to find an archetype with the combined component hash
          const auto target_archetype =
            std::ranges::find_if(ecs_subsystem_st::archetypes,
                                 [combined_hash](const ecs_subsystem_st::archetype_base_ptr_t& a_archetype_base) {
                                   return a_archetype_base->hash() == combined_hash;
                                 });
          if (target_archetype != ecs_subsystem_st::archetypes.end()) {
            const auto current_component_indices = current_archetype.get()->component_indices();
            const auto target_component_indices = target_archetype->get()->component_indices();
            contract_assert(current_component_indices.size() == target_component_indices.size());

            const auto current_component_pointers =
              current_archetype.get()->component_column_data_pointers(current_archetype.get()->m_entities.size());
            const auto target_component_pointers =
              target_archetype->get()->component_column_data_pointers(target_archetype->get()->m_entities.size() + 1);
            contract_assert(current_component_pointers.size() == target_component_pointers.size());

            // transfer the entity
            target_archetype->get()->m_entities.emplace_back(*entity_iterator);
            current_archetype.get()->m_entities.erase(entity_iterator);
            //for (auto i = entity_t{0}; i < )
          }
  }
}
}*/

    namespace global {
      inline auto ecs_subsystem =
        ecs::ecs_subsystem_st {{&diagnostic::log::global::logging_subsystem}, string_view_t {"entity component subsystem"}};
    }
  }
}
