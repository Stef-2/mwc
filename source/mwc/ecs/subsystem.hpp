#pragma once

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_ecs_definition;
import mwc_ecs_component;
import mwc_observer_ptr;

namespace mwc {
  namespace ecs {
    struct ecs_subsystem_st : public subsystem_st {
      using archetype_component_index_map_t = unordered_map_t<archetype_index_t, archetype_component_index_t>;

      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline auto entity_index = entity_t {0};
      static inline auto archetype_index = archetype_index_t {0};

      static inline auto archetype_hash_map = unordered_map_t<archetype_hash_t, archetype_st> {};
      static inline auto entity_archetype_map = unordered_map_t<entity_t, archetype_entity_index_st> {};
      static inline auto component_archetype_map = unordered_map_t<component_index_t, archetype_component_index_map_t> {};
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
    [[nodiscard]] auto generate_entity1(tp_components&&... a_components) {
      if constexpr (not std::is_same_v<tuple_t<tp_components...>, decltype(sorted_component_types<tp_components...>())>) {
        auto newt = sorted_component_types<tp_components...>();
        ((std::get<tp_components>(newt) = a_components), ...);
        auto [... exp] = newt;
        return generate_entity<decltype(exp)...>(std::forward<decltype(exp)>(exp)...);
      }
    }
    template <component_c... tp_components>
    [[nodiscard]] constexpr auto generate_entity(tp_components&&... a_components) -> entity_t
    /*post(r : r != std::numeric_limits<entity_t>::max())*/ {
      // sort components according to their identification in ascending order
      if constexpr (not std::is_same_v<tuple_t<tp_components...>, decltype(sorted_component_types<tp_components...>())>) {
        auto sorted_component_tuple = sorted_component_types<tp_components...>();
        ((std::get<tp_components>(sorted_component_tuple) = a_components), ...);
        auto [... sorted_components] = sorted_component_tuple;
        return generate_entity<decltype(sorted_components)...>(std::forward<decltype(sorted_components)>(sorted_components)...);
      } else {
        constexpr auto hash = archetype_hash<tp_components...>();
        const auto entity = ecs_subsystem_st::entity_index;
        ++ecs_subsystem_st::entity_index;

        // determine if a matching archetype already exists
        const auto archetype_iterator = ecs_subsystem_st::archetype_hash_map.find(hash);
        if (archetype_iterator != ecs_subsystem_st::archetype_hash_map.end()) {
          const auto component_column_index =
            archetype_iterator->second.insert_component_column(std::forward<tp_components>(a_components)...);
          int i;
        }
        return entity;
      }
    } /*
    inline auto destroy_entity(const entity_t a_entity) -> void {
      for (const auto& archetype : ecs_subsystem_st::archetypes) {
        auto& entities = archetype.get()->m_entities;
        const auto entity_iterator = std::ranges::find(entities, a_entity);

        if (entity_iterator != entities.end()) {
          const auto data_column = std::distance(entities.begin(), entity_iterator);
          entities.erase(entity_iterator);
          archetype->remove_component_column(data_column);

          return;
        }
      }
      contract_assert(false);
    }
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
