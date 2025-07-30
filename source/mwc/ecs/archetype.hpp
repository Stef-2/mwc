#pragma once

#include "mwc/core/contract/definition.hpp"

import mwc_definition;
import mwc_ecs_definition;
import mwc_type_mobility;
import mwc_hash;
import mwc_observer_ptr;
import mwc_ecs_component;

import std;

namespace mwc {
  namespace ecs {
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    constexpr auto component_hash(component_hash_t a_initial_hash = 0) -> component_hash_t {
      auto [... sorted_components] = sorted_component_types<tp_components...>();

      const auto lambda = [&a_initial_hash]<component_c tp>() {
        // convert the component type identities to a string format, suitable for hashing
        constexpr auto identity_string = std::bit_cast<array_t<char, sizeof(tp::identity)>>(tp::identity);
        a_initial_hash += polynomial_rolling_hash(string_view_t {identity_string.data(), identity_string.size()});
      };
      (lambda.template operator()<decltype(sorted_components)>(), ...);

      return a_initial_hash;
    }
    inline auto component_hash(component_hash_t a_initial_hash, const span_t<component_index_t> a_component_indices)
      -> component_hash_t {
      for (const auto component_index : a_component_indices)
        a_initial_hash += polynomial_rolling_hash(std::to_string(component_index));

      return a_initial_hash;
    }

    struct archetype_st : public irreproducible_st {
      using component_row_data_t = vector_t<component_storage_st::data_span_t>;
      struct modification_map_st {
        obs_ptr_t<archetype_st> m_insertion;
        obs_ptr_t<archetype_st> m_removal;
      };
      template <component_c... tp_components>
        requires(sizeof...(tp_components) > 0)
      constexpr archetype_st(const archetype_index_t a_index, tp_components&&... a_components)
      : m_component_data {},
        m_entity_count {0},
        m_component_hash {component_hash<tp_components...>()},
        m_index {a_index} {
        m_component_data.reserve(sizeof...(tp_components));
        const auto lambda = [this]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
          (m_component_data.emplace_back(vector_t<byte_t> {}, sizeof(tp_components...[tp_index]),
                                         tp_components...[tp_index] ::identity),
           ...);
        };
        lambda(std::make_index_sequence<sizeof...(tp_components)> {});
        insert_component_row(std::forward<tp_components>(a_components)...);
      }
      archetype_st(const archetype_index_t a_index, const span_t<component_runtime_information_st> a_components)
      : m_component_data {},
        m_entity_count {0},
        m_component_hash {0},
        m_index {a_index} {
        m_component_data.reserve(a_components.size());
        auto component_hash = component_hash_t {0};
        for (const auto& component : a_components) {
          m_component_data.emplace_back(vector_t<byte_t> {}, component.m_component_size, component.m_component_index);
          std::cout << "cd sz: " << component.m_component_size << '\n';
          component_hash += polynomial_rolling_hash(std::to_string(component.m_component_index));
        }
        m_component_hash = component_hash;
      }

      auto component_count() const -> archetype_component_index_t;
      auto entity_count() const -> archetype_entity_index_t;

      template <component_c... tp_components>
        requires(sizeof...(tp_components) > 0)
      constexpr auto component_row(const archetype_entity_index_t a_entity_index) {
        constexpr auto requested_component_hash = component_hash<tp_components...>();
        contract_assert(requested_component_hash == m_component_hash);

        auto [... components] = sorted_component_types<tp_components...>();

        // note: consider returning references rather than values
        const auto lambda = [this, &components..., a_entity_index]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
          ((components...[tp_index] = std::bit_cast<vector_t<std::remove_cvref_t<decltype(components...[tp_index])>>*>(
                                        &m_component_data[tp_index].m_data)
                                        ->operator[](a_entity_index)),
           ...);
        };
        lambda(std::make_index_sequence<sizeof...(tp_components)> {});

        return std::forward_as_tuple(components...);
      }
      constexpr auto component_data_row(const archetype_entity_index_t a_entity_index) -> component_row_data_t {
        auto component_row_data = component_row_data_t {};
        component_row_data.reserve(m_component_data.size());

        // note: consider returning references rather than values
        for (auto& component : m_component_data) {
          auto begin = component.m_data.begin() + component.m_component_size * a_entity_index;
          component_row_data.emplace_back(span_t {begin, component.m_component_size});
        }

        return component_row_data;
      }
      template <component_c... tp_components>
      constexpr auto insert_component_row(tp_components&&... a_components) {
        static_assert(std::is_same_v<tuple_t<tp_components...>, decltype(sorted_component_types<tp_components...>())>);
        contract_assert(sizeof...(tp_components) == component_count());

        auto lambda = [&, this]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
          (m_component_data[tp_index].m_data.append_range(
             std::bit_cast<array_t<byte_t, sizeof(a_components...[tp_index])>>(a_components...[tp_index])),
           ...);
        };
        lambda(std::make_index_sequence<sizeof...(tp_components)> {});
        ++m_entity_count;

        return m_entity_count;
      }
      auto insert_component_data_row(const vector_t<component_storage_view_st>& a_components) {
        contract_assert(a_components.size() == component_count());

        for (auto i = archetype_component_index_t {0}; i < a_components.size(); ++i)
          if (m_component_data[i].m_component_index == a_components[i].m_component_index)
            m_component_data[i].m_data.append_range(a_components[i].m_data_span);
        ++m_entity_count;

        return m_entity_count;
      }
      auto remove_component_row(const archetype_entity_index_t a_entity_index) {
        std::cout << "cc: " << m_component_data.size() << '\n';
        for (auto& component : m_component_data) {
          // note: defined behaviour ?
          const auto begin = component.m_data.begin() + a_entity_index * component.m_component_size;
          const auto end = begin + component.m_component_size;
          component.m_data.erase(begin, end);
        }
        --m_entity_count;

        return m_entity_count;
      }

      constexpr auto operator<=>(const archetype_st& a_other) const;

      vector_t<component_storage_st> m_component_data;
      archetype_entity_index_t m_entity_count;
      component_hash_t m_component_hash;
      archetype_index_t m_index;
      unordered_map_t<component_hash_t, modification_map_st> m_modification_map;
    };
    struct archetype_entity_index_st {
      obs_ptr_t<archetype_st> m_archetype;
      entity_index_t m_entity_index;
    };
    struct archetype_component_index_st {
      obs_ptr_t<archetype_st> m_archetype;
      archetype_component_index_t m_component_index;
    };
  }
}