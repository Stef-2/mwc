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
    template <component_c... tps>
      requires(sizeof...(tps) > 0)
    consteval auto archetype_hash() -> archetype_hash_t {
      auto combined_hash = archetype_hash_t {0};

      const auto lambda = [&combined_hash]<component_c tp>() {
        // convert the component type identities to a string format, suitable for hashing
        constexpr auto identity_string = std::bit_cast<array_t<char, sizeof(tp::identity)>>(tp::identity);
        combined_hash += polynomial_rolling_hash(string_view_t {identity_string.data(), identity_string.size()});
      };

      (lambda.template operator()<tps>(), ...);

      return combined_hash;
    }

    struct archetype_st : public irreproducible_st {
      struct modification_map_st {
        archetype_st* m_insertion;
        archetype_st* m_removal;
      };

      template <component_c... tp_components>
      constexpr archetype_st(const archetype_index_t a_index)
      : m_component_data {},
        m_element_count {0},
        m_hash {archetype_hash<tp_components...>()},
        m_index {a_index} {}

      template <component_c... tp_components>
      constexpr auto insert_component_column(tp_components&&... a_components) {
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
      constexpr auto operator<=>(const archetype_st& a_other) const;

      vector_t<component_storage_st> m_component_data;
      size_t m_element_count;
      archetype_hash_t m_hash;
      archetype_index_t m_index;
      //unordered_map_t<component_index_t, modification_map_st> m_modification_map;
    };
    struct archetype_entity_index_st {
      archetype_st& m_archetype;
      entity_t m_entity_index;
    };
    struct archetype_component_index_st {
      archetype_component_index_t m_component_index;
    };
  }
}