#pragma once

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"

import mwc_ctti;
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

    template <component_c tp_component_x, component_c tp_component_y>
    consteval auto sort_components() {
      if constexpr (tp_component_x::identity > tp_component_y::identity)
        return pair_t<tp_component_x, tp_component_y> {};
      else
        return pair_t<tp_component_y, tp_component_x> {};
    }
    template <bool b, component_c tp_x, component_c... tps>
    consteval auto sorted() -> bool {
      if constexpr (not b or sizeof...(tps) == 0)
        return b;
      else if constexpr (tp_x::identity > tps...[0] ::identity)
        return sorted<b, tps...>();
      else
        return sorted<false, tps...>();
    }
    template <typename tp_tuple, component_c tp_x, component_c tp_y, component_c... tps>
    consteval auto sort() {
      if constexpr (sizeof...(tps) == 0) {
        using result_t = decltype(std::tuple_cat(tp_tuple {}, sort_components<tp_x, tp_y>()));
        auto [... unpack] = result_t {};
        if constexpr (not sorted<true, decltype(unpack)...>()) {
          return sort<tuple_t<>, decltype(unpack)...>();
        } else {
          //auto [... result] = result_t {};
          /*if constexpr (sorted<true, decltype(result)...>())
            return sort<tuple_t<>, tp_x, tp_y, tps...>();
          else*/
          return std::tuple_cat(tp_tuple {}, sort_components<tp_x, tp_y>());
        }
      }
      /*if constexpr (sizeof...(tps) == 1)
        return std::tuple_cat(tp_tuple {}, sort_components<tp_x, tp_y>());*/

      if constexpr (sizeof...(tps) > 0) {
        using t = decltype(sort_components<tp_x, tp_y>());
        using combined_t = decltype(std::tuple_cat(tp_tuple {}, tuple_t<decltype(t::first)> {}));
        using rest_t = decltype(std::tuple_cat(tuple_t<decltype(t::second)> {}, tuple_t<tps...> {}));
        auto [... rest] = rest_t {};
        //static_assert((std::is_same_v<decltype(rest...[1]), void>));
        //static_assert(sizeof...(rest) == 4);
        //return sort<combined_t, decltype(rest)...>();
        return sort<combined_t, decltype(rest)...>();
      }
    }
    template <component_c tp_x, component_c... tps>
    consteval auto min() {
      constexpr auto m = std::max({tp_x::identity, tps::identity...});
      return decltype(ctti::observe_type_list<component_type_list_st>())::component_at_index_t<m> {};
    }
    template <typename tp_tuple, component_c tp_x, component_c... tps>
    consteval auto bige_sort() {
      //static_assert(sorted<true, tp_x, tps...>());
      using t1 = decltype(sort<tp_tuple, tp_x, tps...>());
      //using t2 = decltype(sort<t1, tp_x, tps...>());
      //using t3 = decltype(sort<t2, tp_x, tps...>());

      static_assert(std::is_same_v<char, t1>);
    }
    template </*component_c tp_component, */ component_c... tp_components>
    auto generate_archetype() -> void {
      using t = decltype(bige_sort<tuple_t<>, tp_components...>());
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
    }

    namespace global {
      inline auto ecs_subsystem =
        ecs::ecs_subsystem_st {{&diagnostic::log::global::logging_subsystem}, string_view_t {"entity component subsystem"}};
    }
  }
}
