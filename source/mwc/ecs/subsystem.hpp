#pragma once

#include "mwc/core/contract/definition.hpp"

#include "archetype.hpp"
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
            archetype_entity_index_st {&archetype_iterator->second, archetype_iterator->second.entity_count() - 1});
          // a matching archetype does not exist, create one
        } else {
          const auto archetype_index = ecs_subsystem_st::archetype_index;
          ++ecs_subsystem_st::archetype_index;

          archetype_iterator = ecs_subsystem_st::hash_archetype_map
                                 .emplace(hash, archetype_st {archetype_index, std::forward<tp_components>(a_components)...})
                                 .first;
          // register entity - archetype mapping
          mwc::ecs::ecs_subsystem_st::entity_archetype_map.emplace(
            entity_index,
            archetype_entity_index_st {&archetype_iterator->second, archetype_iterator->second.entity_count() - 1});
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
      const auto& archetype = ecs_subsystem_st::entity_archetype_map[m_entity_index];
      contract_assert(archetype.m_archetype != nullptr and m_entity_index != invalid_entity);

      return archetype.m_archetype->component_row<tp_components...>(archetype.m_entity_index);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    inline auto insert_components(const entity_index_t a_entity_index, tp_components&&... a_components) -> void {
      // determine the current archetype
      const auto source_archetype = ecs_subsystem_st::entity_archetype_map[a_entity_index];
      contract_assert(source_archetype.m_archetype != nullptr and a_entity_index != invalid_entity);

      std::cout << "sz: " << ecs_subsystem_st::hash_archetype_map.size() << std::endl;

      constexpr auto inserted_components_hash = component_hash<tp_components...>();
      // determine if this class of component modification has been recorded before
      if (source_archetype.m_archetype->m_modification_map.contains(inserted_components_hash)) [[likely]] {
        auto& target_archetype = *source_archetype.m_archetype->m_modification_map[inserted_components_hash].m_insertion;

        // register new component data for transfer and record the entity row index
        auto target_component_data = vector_t<component_storage_view_st> {};
        target_component_data.reserve(sizeof...(tp_components) + source_archetype.m_archetype->component_count());

        (target_component_data.emplace_back(component_storage_view_st {
         .m_data_span = {span_t<byte_t, sizeof(a_components)> {std::bit_cast<byte_t*>(&a_components), sizeof(a_components)}},
         .m_component_index = std::remove_cvref_t<decltype(a_components)>::identity}),
         ...);

        //const auto target_entity_index = target_archetype.insert_component_row(std::forward<tp_components>(a_components)...) - 1;
        const auto& source_entity_index = source_archetype.m_entity_index;
        // move each component from source to target archetype
        for (auto i = archetype_component_index_t {0}; i < source_archetype.m_archetype->component_count(); ++i) {
          auto& source_component_storage = source_archetype.m_archetype->m_component_data[i];
          const auto& target_component_column =
            ecs_subsystem_st::component_archetype_map.find(source_component_storage.m_component_index)->second.m_component_index;
          auto& target_component_storage = target_archetype.m_component_data[target_component_column];

          const auto source_data_begin =
            source_component_storage.m_data.begin() + source_entity_index * source_component_storage.m_component_size;
          const auto source_data_end = source_data_begin + source_component_storage.m_component_size;

          // register source component data for transfer
          target_component_data.emplace_back(
            component_storage_view_st {.m_data_span = {source_data_begin, source_data_end},
                                       .m_component_index = source_component_storage.m_component_index});
          /*target_component_storage.m_data.insert_range(target_component_storage.m_data.begin() + target_entity_index,
                                                       std::ranges::subrange {source_data_begin, source_data_end});*/
          // remove component data from the source archetype
          source_component_storage.m_data.erase(source_data_begin, source_data_end);
        }
        target_archetype.insert_component_data_row(target_component_data);
        // register the entity with the target archetype
        ecs_subsystem_st::entity_archetype_map[a_entity_index] =
          archetype_entity_index_st {.m_archetype = &target_archetype, .m_entity_index = target_archetype.entity_count()};
        return;
      }
      // no record of this class of component modification exists, possible reasons:
      // this is the first time an entity is making this kind of archetype transition, but the target archetype exists
      // the target archetype doesn't exist
      else [[unlikely]] {
        const auto combined_hash = component_hash<tp_components...>(source_archetype.m_archetype->m_component_hash);
        // determine if the target archetype exists
        const auto target_archetype_iterator = ecs_subsystem_st::hash_archetype_map.find(combined_hash);
        if (target_archetype_iterator != ecs_subsystem_st::hash_archetype_map.end()) {
          // target archetype exists, record this class of component modification and call again
          auto& target_archetype = target_archetype_iterator->second;
          constexpr auto input_component_hash = component_hash<tp_components...>();
          source_archetype.m_archetype->m_modification_map.emplace(
            input_component_hash,
            archetype_st::modification_map_st {.m_insertion = &target_archetype});
          target_archetype.m_modification_map.emplace(
            input_component_hash,
            archetype_st::modification_map_st {.m_removal = source_archetype.m_archetype});

          return insert_components(a_entity_index, std::forward<tp_components>(a_components)...);
        }
        // the target archetype doesn't exist, generate one and call again
        else {
          auto component_runtime_information = std::vector<component_runtime_information_st> {};
          component_runtime_information.reserve(source_archetype.m_archetype->component_count() + sizeof...(tp_components));

          const auto lambda = [&component_runtime_information]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
            (component_runtime_information.emplace_back(
               component_runtime_information_st {.m_component_size = sizeof(tp_components...[tp_index]),
                                                 .m_component_index = tp_components...[tp_index] ::identity}),
             ...);
          };
          lambda(std::make_index_sequence<sizeof...(tp_components)> {});
          for (const auto& source_component : source_archetype.m_archetype->m_component_data) {
            component_runtime_information.emplace_back(
              component_runtime_information_st {.m_component_size = source_component.m_component_size,
                                                .m_component_index = source_component.m_component_index});
          }
          ecs_subsystem_st::hash_archetype_map.emplace(
            combined_hash,
            archetype_st {ecs_subsystem_st::archetype_index++, component_runtime_information});

          return insert_components(a_entity_index, std::forward<tp_components>(a_components)...);
        }
      }
      std::unreachable();
      contract_assert(false);
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
