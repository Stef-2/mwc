#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/ecs/archetype.hpp"
#include "mwc/ecs/definition.hpp"
#include "mwc/ecs/component.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"

import mwc_definition;
import mwc_subsystem;
//import mwc_ecs_definition;
//import mwc_ecs_component;
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
    struct ecs_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline auto entity_index = entity_index_t {1};
      static inline auto archetype_index = archetype_index_t {1};

      static inline auto hash_archetype_map = unordered_map_t<component_hash_t, archetype_st> {};
      static inline auto entity_archetype_map = unordered_map_t<entity_index_t, archetype_entity_index_st> {};
      static inline auto component_archetype_map =
        unordered_multimap_t<component_index_t, unordered_map_t<obs_ptr_t<archetype_st>, archetype_component_index_t>> {};
    };

    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    constexpr auto generate_archetype(const entity_index_t a_entity_index, tp_components&&... a_components) -> void {
      // determine if the components are sorted according to their identification
      if constexpr (not std::is_same_v<tuple_t<tp_components...>, decltype(sorted_component_types<tp_components...>())>) {
        auto sorted_component_tuple = sorted_component_types<tp_components...>();
        ((std::get<tp_components>(sorted_component_tuple) = a_components), ...);
        auto [... sorted_components] = sorted_component_tuple;
        return generate_archetype<decltype(sorted_components)...>(std::forward<decltype(sorted_components)>(sorted_components)...);
      } else {
        constexpr auto hash = component_hash<std::plus<>, tp_components...>();
        const auto archetype_index = ecs_subsystem_st::archetype_index;
        ++ecs_subsystem_st::archetype_index;

        const auto archetype_iterator =
          ecs_subsystem_st::hash_archetype_map
            .emplace(hash, archetype_st {archetype_index, std::forward<tp_components>(a_components)...})
            .first;
        // register entity - archetype mapping
        mwc::ecs::ecs_subsystem_st::entity_archetype_map.emplace(a_entity_index,
                                                                 archetype_entity_index_st {&archetype_iterator->second, 0});
        // register component - archetype mapping
        const auto lambda = [&archetype_iterator, &archetype_index]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
          ((ecs_subsystem_st::component_archetype_map.emplace(
             tp_components... [tp_index] ::identity,
             unordered_map_t<obs_ptr_t<archetype_st>, archetype_component_index_t> {
               pair_t {
                 &archetype_iterator->second, tp_index
               }
             })),
           ...);
        };
        lambda(std::make_index_sequence<sizeof...(tp_components)> {});
      }
    }
    inline auto generate_archetype(const component_hash_t a_component_hash,
                                   const span_t<component_runtime_information_st>
                                     a_runtime_component_information)
      -> void pre(std::ranges::is_sorted(a_runtime_component_information, std::less {})) {
      const auto generated_archetype_ptr =
        &ecs_subsystem_st::hash_archetype_map
           .emplace(a_component_hash, archetype_st {ecs_subsystem_st::archetype_index++, a_runtime_component_information})
           .first->second;
      // register components with the new archetype
      for (auto i = archetype_component_index_t {}; i < a_runtime_component_information.size(); ++i)
        ecs_subsystem_st::component_archetype_map.emplace(
          a_runtime_component_information[i].m_component_index,
          unordered_map_t<archetype_st*, archetype_component_index_t> {{generated_archetype_ptr, i}});
    }
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
        constexpr auto hash = component_hash<std::plus<>, tp_components...>();
        const auto entity_index = ecs_subsystem_st::entity_index;
        ++ecs_subsystem_st::entity_index;

        // determine if a matching archetype already exists
        auto archetype_iterator = ecs_subsystem_st::hash_archetype_map.find(hash);
        if (archetype_iterator != ecs_subsystem_st::hash_archetype_map.end()) [[likely]] {
          // insert component data into the archetype
          archetype_iterator->second.insert_component_row(std::forward<tp_components>(a_components)...);
          // register entity - archetype mapping
          ecs_subsystem_st::entity_archetype_map.emplace(
            entity_index,
            archetype_entity_index_st {&archetype_iterator->second, archetype_iterator->second.entity_count() - 1});
          // a matching archetype does not exist, create one
        } else [[unlikely]] {
          generate_archetype(entity_index, std::forward<tp_components>(a_components)...);
        }
        return entity_index;
      }
    }
    inline auto destroy_entity(const entity_index_t a_entity)
      -> void pre(ecs_subsystem_st::entity_archetype_map.contains(a_entity)) {
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
    constexpr auto
    register_archetype_modification(const component_hash_t a_component_hash, archetype_st& a_removal, archetype_st& a_insertion) {
      auto& removal_modification_map = a_removal.m_modification_map[a_component_hash];
      contract_assert(removal_modification_map.m_insertion == nullptr);
      removal_modification_map.m_insertion = &a_insertion;

      auto& insertion_modification_map = a_insertion.m_modification_map[a_component_hash];
      contract_assert(insertion_modification_map.m_removal == nullptr);
      insertion_modification_map.m_removal = &a_removal;
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    inline auto insert_components(const entity_index_t a_entity_index, tp_components&&... a_components) -> void {
      // determine the source archetype
      const auto source_archetype = ecs_subsystem_st::entity_archetype_map[a_entity_index];
      contract_assert(source_archetype.m_archetype != nullptr and a_entity_index != invalid_entity);

      constexpr auto inserted_components_hash = component_hash<std::plus<>, tp_components...>();
      // determine if this class of component modification has been recorded before
      if (source_archetype.m_archetype->m_modification_map.contains(inserted_components_hash)) [[likely]] {
        auto& target_archetype = *source_archetype.m_archetype->m_modification_map[inserted_components_hash].m_insertion;

        // register new component data for transfer
        auto target_component_data = vector_t<component_storage_view_st> {};
        target_component_data.reserve(sizeof...(tp_components) + source_archetype.m_archetype->component_count());

        (target_component_data.emplace_back(component_storage_view_st {
         .m_data_span = {span_t<byte_t, sizeof(a_components)> {std::bit_cast<byte_t*>(&a_components), sizeof(a_components)}},
         .m_component_index = std::remove_cvref_t<decltype(a_components)>::identity}),
         ...);

        const auto& source_entity_index = source_archetype.m_entity_index;
        // register each existing components in the source archetype for transfer
        for (auto i = archetype_component_index_t {0}; i < source_archetype.m_archetype->component_count(); ++i) {
          auto& source_component_storage = source_archetype.m_archetype->m_component_data[i];
          const auto& target_component_column = target_archetype.component_index(source_component_storage.m_component_index);
          auto& target_component_storage = target_archetype.m_component_data[target_component_column];

          const auto source_data_begin =
            source_component_storage.m_data.begin() + source_entity_index * source_component_storage.m_component_size;
          const auto source_data_end = source_data_begin + source_component_storage.m_component_size;

          // register source component data for transfer
          target_component_data.emplace_back(
            component_storage_view_st {.m_data_span = {source_data_begin, source_data_end},
                                       .m_component_index = source_component_storage.m_component_index});
        }
        // insert combined component data into the target archetype
        target_archetype.insert_component_data_row(target_component_data);
        // remove component data from the source archetype
        source_archetype.m_archetype->remove_component_row(source_entity_index);
        // register the entity with the target archetype
        ecs_subsystem_st::entity_archetype_map[a_entity_index] =
          archetype_entity_index_st {.m_archetype = &target_archetype, .m_entity_index = target_archetype.entity_count() - 1};
        return;
      }
      // no record of this class of component modification exists, possible reasons:
      // 1: this is the first time an entity is making this kind of archetype transition, but the target archetype exists
      // 2: the target archetype doesn't exist
      else [[unlikely]] {
        const auto combined_hash = component_hash<std::plus<>, tp_components...>(source_archetype.m_archetype->m_component_hash);
        // determine if the target archetype exists
        const auto target_archetype_iterator = ecs_subsystem_st::hash_archetype_map.find(combined_hash);
        if (target_archetype_iterator != ecs_subsystem_st::hash_archetype_map.end()) {
          // target archetype exists, record this class of component modification and call again
          auto& target_archetype = target_archetype_iterator->second;
          constexpr auto input_component_hash = component_hash<std::plus<>, tp_components...>();

          register_archetype_modification(input_component_hash, *source_archetype.m_archetype, target_archetype);

          // call again
          return insert_components(a_entity_index, std::forward<tp_components>(a_components)...);
        }
        // the target archetype doesn't exist, generate one and call again
        else {
          auto component_runtime_information = vector_t<component_runtime_information_st> {};
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
          // sort the component runtime information according to component indices
          std::ranges::sort(component_runtime_information, std::less {});

          // generate a new archetype
          const auto generated_archetype_ptr =
            &ecs_subsystem_st::hash_archetype_map
               .emplace(combined_hash, archetype_st {ecs_subsystem_st::archetype_index++, component_runtime_information})
               .first->second;
          // register components with the new archetype
          for (auto i = archetype_component_index_t {}; i < component_runtime_information.size(); ++i)
            ecs_subsystem_st::component_archetype_map.emplace(
              component_runtime_information[i].m_component_index,
              unordered_map_t<archetype_st*, archetype_component_index_t> {{generated_archetype_ptr, i}});

          // call again
          return insert_components(a_entity_index, std::forward<tp_components>(a_components)...);
        }
      }
      std::unreachable();
      contract_assert(false);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    inline auto remove_components(const entity_index_t a_entity_index) -> void {
      const auto source_archetype = ecs_subsystem_st::entity_archetype_map[a_entity_index];
      contract_assert(source_archetype.m_archetype != nullptr and a_entity_index != invalid_entity);

      constexpr auto inserted_components_hash = component_hash<std::plus<>, tp_components...>();
      // determine if this class of component modification has been recorded before
      if (source_archetype.m_archetype->m_modification_map.contains(inserted_components_hash)) [[likely]] {
        auto& target_archetype = *source_archetype.m_archetype->m_modification_map[inserted_components_hash].m_insertion;
        const auto& source_entity_index = source_archetype.m_entity_index;

        // transfer common components from source to target archetype
        for (auto i = archetype_component_index_t {0}; i < target_archetype.component_count(); ++i) {
          auto& target_component_storage = target_archetype.m_component_data[i];
          const auto& source_component_column =
            source_archetype.m_archetype->component_index(target_component_storage.m_component_index);
          auto& source_component_storage = source_archetype.m_archetype->m_component_data[source_component_column];

          const auto source_data_begin =
            source_component_storage.m_data.begin() + source_entity_index * source_component_storage.m_component_size;
          const auto source_data_end = source_data_begin + source_component_storage.m_component_size;

          target_archetype.m_component_data[i].m_data.append_range(std::ranges::subrange {source_data_begin, source_data_end});
        }
        // remove component data from the source archetype
        source_archetype.m_archetype->remove_component_row(source_entity_index);
        // target archetype received a row of components
        ++target_archetype.m_entity_count;
        // register the entity with the target archetype
        ecs_subsystem_st::entity_archetype_map[a_entity_index] =
          archetype_entity_index_st {.m_archetype = &target_archetype, .m_entity_index = target_archetype.entity_count() - 1};
        return;
      }
      // no record of this class of component modification exists, possible reasons:
      // 1: this is the first time an entity is making this kind of archetype transition, but the target archetype exists
      // 2: the target archetype doesn't exist
      else [[unlikely]] {
        const auto combined_hash = component_hash<std::minus<>, tp_components...>(source_archetype.m_archetype->m_component_hash);
        // determine if the target archetype exists
        const auto target_archetype_iterator = ecs_subsystem_st::hash_archetype_map.find(combined_hash);
        if (target_archetype_iterator != ecs_subsystem_st::hash_archetype_map.end()) {
          // target archetype exists, record this class of component modification and call again
          auto& target_archetype = target_archetype_iterator->second;
          constexpr auto input_component_hash = component_hash<std::plus<>, tp_components...>();

          register_archetype_modification(input_component_hash, *source_archetype.m_archetype, target_archetype);

          // call again
          return remove_components<tp_components...>(a_entity_index);
        }
        // the target archetype doesn't exist, generate one and call again
        else {
          auto component_runtime_information = vector_t<component_runtime_information_st> {};
          component_runtime_information.reserve(source_archetype.m_archetype->component_count() - sizeof...(tp_components));

          const auto lambda = [&component_runtime_information,
                               &source_archetype]<component_c tp_component, component_c... tps>(this auto&& a_this) -> void {
            for (const auto& source_component : source_archetype.m_archetype->m_component_data)
              if (source_component.m_component_index != tp_component::identity)
                component_runtime_information.emplace_back(
                  component_runtime_information_st {.m_component_size = source_component.m_component_size,
                                                    .m_component_index = source_component.m_component_index});
            if constexpr (sizeof...(tps) > 0)
              return a_this.template operator()<tps...>();
          };
          lambda.template operator()<tp_components...>();

          // sort the component runtime information according to component indices
          std::ranges::sort(component_runtime_information,
                            [](const auto& a_x, const auto& a_y) { return a_x.m_component_index < a_y.m_component_index; });
          // generate a new archetype
          const auto generated_archetype_ptr =
            &ecs_subsystem_st::hash_archetype_map
               .emplace(combined_hash, archetype_st {ecs_subsystem_st::archetype_index++, component_runtime_information})
               .first->second;
          // register components with the new archetype
          for (auto i = archetype_component_index_t {}; i < component_runtime_information.size(); ++i)
            ecs_subsystem_st::component_archetype_map.emplace(
              component_runtime_information[i].m_component_index,
              unordered_map_t<archetype_st*, archetype_component_index_t> {{generated_archetype_ptr, i}});

          // call again
          return remove_components<tp_components...>(a_entity_index);
        }
      }
      std::unreachable();
      contract_assert(false);
    }

    namespace global {
      inline auto ecs_subsystem =
        ecs::ecs_subsystem_st {{&diagnostic::log::global::logging_subsystem}, string_view_t {"entity component subsystem"}};
    }
  }
}
