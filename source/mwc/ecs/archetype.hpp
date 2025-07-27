#pragma once

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
    consteval auto component_hash() -> component_hash_t {
      auto [... sorted_components] = sorted_component_types<tp_components...>();

      auto combined_hash = component_hash_t {0};

      const auto lambda = [&combined_hash]<component_c tp>() {
        // convert the component type identities to a string format, suitable for hashing
        constexpr auto identity_string = std::bit_cast<array_t<char, sizeof(tp::identity)>>(tp::identity);
        combined_hash += polynomial_rolling_hash(string_view_t {identity_string.data(), identity_string.size()});
      };

      (lambda.template operator()<decltype(sorted_components)>(), ...);

      return combined_hash;
    }

    struct archetype_st : public irreproducible_st {
      using component_row_data_t = vector_t<span_t<byte_t>>;
      struct modification_map_st {
        archetype_st* m_insertion;
        archetype_st* m_removal;
      };
      template <component_c... tp_components>
        requires(sizeof...(tp_components) > 0)
      constexpr archetype_st(const archetype_index_t a_index, tp_components&&... a_components)
      : m_component_data {},
        m_element_count {0},
        m_hash {component_hash<tp_components...>()},
        m_index {a_index} {
        m_component_data.resize(sizeof...(tp_components));
        // initialize type erased component vectors
        auto lambda = [&, this]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
          ((m_component_data[tp_index].m_data = new vector_t<tp_components... [tp_index]> {}), ...);
        };
        lambda(std::make_index_sequence<sizeof...(tp_components)> {});

        insert_component_row(std::forward<tp_components>(a_components)...);
      }
      ~archetype_st();
      archetype_st(archetype_st&&) noexcept = default;
      auto operator=(archetype_st&&) noexcept -> archetype_st& = default;

      template <component_c... tp_components>
      constexpr auto component_row(const archetype_entity_index_t a_entity_index) {
        constexpr auto requested_component_hash = component_hash<tp_components...>();
        contract_assert(requested_component_hash == m_hash);

        auto [... components] = sorted_component_types<tp_components...>();

        const auto lambda = [this, &components..., a_entity_index]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
          ((components...[tp_index] = std::bit_cast<vector_t<std::remove_reference_t<decltype(components...[tp_index])>>*>(
                                        m_component_data[tp_index].m_data)
                                        ->operator[](a_entity_index)),
           ...);
        };
        lambda(std::make_index_sequence<sizeof...(tp_components)> {});

        return std::forward_as_tuple(components...);
      }
      constexpr auto component_data_row(const archetype_entity_index_t a_entity_index) -> component_row_data_t {
        auto component_row_data = component_row_data_t {};
        component_row_data.reserve(m_component_data.size());

        for (const auto& component : m_component_data) {
          const auto component_vector = std::bit_cast<vector_t<byte_t>*>(component.m_data);
          const auto begin = component_vector->begin() + component.m_data_size * a_entity_index;
          component_row_data.emplace_back(span_t {begin, component.m_data_size});
        }

        return component_row_data;
      }
      template <component_c... tp_components>
      constexpr auto insert_component_row(tp_components&&... a_components) {
        static_assert(std::is_same_v<tuple_t<tp_components...>, decltype(sorted_component_types<tp_components...>())>);

        auto lambda = [&, this]<size_t... tp_index>(std::index_sequence<tp_index...>) -> void {
          (std::bit_cast<vector_t<tp_components...[tp_index]>*>(m_component_data[tp_index].m_data) -> emplace_back(
                                                                                                     a_components...[tp_index]),
           ...);
        };
        lambda(std::make_index_sequence<sizeof...(tp_components)> {});
        ++m_element_count;

        return m_element_count;
      }
      auto remove_component_row(const archetype_entity_index_t a_entity_index) {
        ++m_element_count;
        for (const auto& component_vector : m_component_data) {
          // note: defined behaviour ?
          auto type_erased_vector = std::bit_cast<vector_t<byte_t>*>(component_vector.m_data);
          const auto begin = type_erased_vector->begin() + a_entity_index * component_vector.m_data_size;
          const auto end = begin + component_vector.m_data_size;
          type_erased_vector->erase(begin, end);
          type_erased_vector->resize(m_element_count);
        }
      }

      constexpr auto operator<=>(const archetype_st& a_other) const;

      vector_t<component_storage_st> m_component_data;
      size_t m_element_count;
      component_hash_t m_hash;
      archetype_index_t m_index;
      unordered_map_t<component_hash_t, modification_map_st> m_modification_map;
    };
    struct archetype_entity_index_st {
      archetype_st* m_archetype;
      entity_index_t m_entity_index;
    };
    struct archetype_component_index_st {
      archetype_st* m_archetype;
      archetype_component_index_t m_component_index;
    };
  }
}