#pragma once

import mwc_definition;
import mwc_ecs_definition;
import mwc_type_mobility;

import std;

namespace mwc {
  namespace ecs {
    template <typename... tp_components>
      requires((tp_components::identity < std::numeric_limits<component_index_t>::max()), ...)
    class archetype_ct : public immobile_st {
      public:
      using entity_storage_t = vector_t<entity_t>;
      using component_storage_t = tuple_t<vector_t<tp_components>...>;

      static constexpr auto component_indices = array_t<component_index_t, sizeof...(tp_components)> {tp_components::identity...};

      struct configuration_st {
        static constexpr auto default_configuration() -> configuration_st;

        size_t m_entity_storage_capacity;
      };

      archetype_ct(const configuration_st& a_configuration = configuration_st::default_configuration());

      template <typename tp_this>
      [[nodiscard]] constexpr auto configuration(this tp_this&& a_this) -> decltype(auto);

      //private:
      entity_storage_t m_entities;
      component_storage_t m_components;
      configuration_st m_configuration;
    };

    // implementation
    template <typename... tp_components>
      requires((tp_components::identity < std::numeric_limits<component_index_t>::max()), ...)
    constexpr auto archetype_ct<tp_components...>::configuration_st::default_configuration() -> configuration_st {
      return configuration_st {.m_entity_storage_capacity = 32};
    }
    template <typename... tp_components>
      requires((tp_components::identity < std::numeric_limits<component_index_t>::max()), ...)
    template <typename tp_this>
    constexpr auto archetype_ct<tp_components...>::configuration(this tp_this&& a_this) -> decltype(auto) {
      return std::forward_like<decltype(a_this)>(a_this.m_configuration);
    }
    template <typename... tp_components>
      requires((tp_components::identity < std::numeric_limits<component_index_t>::max()), ...)
    archetype_ct<tp_components...>::archetype_ct(const configuration_st& a_configuration)
    : m_entities {},
      m_components {},
      m_configuration {a_configuration} {
      m_entities.reserve(m_configuration.m_entity_storage_capacity);
      std::apply([this](auto&... a_element) { (a_element.reserve(m_configuration.m_entity_storage_capacity), ...); },
                 m_components);
    }
  }
}
