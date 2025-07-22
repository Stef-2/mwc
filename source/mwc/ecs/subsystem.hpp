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
      using archetype_base_ptr_t = std::unique_ptr<archetype_base_st>;
      using archetype_storage_t = vector_t<archetype_base_ptr_t>;

      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline auto entity_index = entity_t {0};
      static inline auto archetypes = archetype_storage_t {};
    };

    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto generate_archetype() -> void {
      ecs_subsystem_st::archetypes.emplace_back(std::make_unique<archetype_ct<tp_components...>>());
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto destroy_archetype() -> void {
      constexpr auto hash = archetype_hash<tp_components...>();
      for (auto i = 0uz; i < ecs_subsystem_st::archetypes.size(); ++i)
        if (ecs_subsystem_st::archetypes[i].get()->hash() == hash)
          ecs_subsystem_st::archetypes.erase(ecs_subsystem_st::archetypes.begin() + i);
    }

    template <component_c... tp_components>
    [[nodiscard]] auto generate_entity(tp_components&&... a_components) -> entity_t
      post(r : r != std::numeric_limits<entity_t>::max()) {
      constexpr auto hash = archetype_hash<tp_components...>();
      const auto entity = ecs_subsystem_st::entity_index;
      ++ecs_subsystem_st::entity_index;

      // determine if a matching archetype already exists
      const auto archetype_iterator =
        std::ranges::find_if(ecs_subsystem_st::archetypes, [](const ecs_subsystem_st::archetype_base_ptr_t& a_archetype_base) {
          return a_archetype_base->hash() == hash;
        });

      auto archetype = obs_ptr_t<archetype_ct<tp_components...>> {};
      if (archetype_iterator != ecs_subsystem_st::archetypes.end()) {
        archetype = static_cast<archetype_ct<tp_components...>*>(archetype_iterator->get());
      } else {
        generate_archetype<tp_components...>();
        archetype = static_cast<archetype_ct<tp_components...>*>(ecs_subsystem_st::archetypes.back().get());
      }

      archetype->m_entities.emplace_back(entity);
      archetype->insert_component_column(std::forward<tp_components>(a_components)...);

      return entity;
    }
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
    auto entity_insert_components(const entity_t a_entity, tp_components&&... a_components) {
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
          const auto archetype_iterator = std::ranges::find_if(
            ecs_subsystem_st::archetypes, [combined_hash](const ecs_subsystem_st::archetype_base_ptr_t& a_archetype_base) {
              return a_archetype_base->hash() == combined_hash;
            });
          if (archetype_iterator != ecs_subsystem_st::archetypes.end()) {
            ;
            ;
          }
        }
      }
    }

    namespace global {
      inline auto ecs_subsystem =
        ecs::ecs_subsystem_st {{&diagnostic::log::global::logging_subsystem}, string_view_t {"entity component subsystem"}};
    }
  }
}
