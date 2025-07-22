#pragma once

import mwc_definition;
import mwc_ecs_definition;
import mwc_type_mobility;
import mwc_hash;
import mwc_observer_ptr;
import mwc_static_string;
import mwc_ecs_component;

import std;

namespace mwc {
  namespace ecs {
    template <component_c... tps>
      requires(sizeof...(tps) > 0)
    consteval auto archetype_hash() -> archetype_hash_t {
      auto combined_hash = archetype_hash_t {0};

      const auto lambda = [&combined_hash]<component_c tp>() {
        constexpr char identity_string[sizeof(component_index_t)] = {tp::identity};

        combined_hash += hash<static_string_st {identity_string}>();
      };

      (lambda.template operator()<tps>(), ...);

      return combined_hash;
    }

    struct archetype_base_st {
      virtual ~archetype_base_st() = default;

      [[nodiscard]] virtual constexpr auto hash() const -> archetype_hash_t = 0;
    };

    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    class archetype_ct : public archetype_base_st, public irreproducible_st {
      public:
      using entity_storage_t = vector_t<entity_t>;
      using component_storage_t = tuple_t<vector_t<tp_components>...>;

      struct configuration_st {
        static constexpr auto default_configuration() -> configuration_st;

        size_t m_component_storage_capacity;
      };

      archetype_ct(const configuration_st& a_configuration = configuration_st::default_configuration());

      [[nodiscard]] constexpr auto hash() const -> archetype_hash_t override final;
      auto insert_entity(const entity_t a_entity, tp_components&&... a_components)
        -> void pre(not std::ranges::contains(m_entities, a_entity));

      template <typename tp_this>
      [[nodiscard]] constexpr auto configuration(this tp_this&& a_this) -> decltype(auto);

      //private:
      entity_storage_t m_entities;
      component_storage_t m_components;
      configuration_st m_configuration;
    };

    // implementation
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    constexpr auto archetype_ct<tp_components...>::configuration_st::default_configuration() -> configuration_st {
      return configuration_st {.m_component_storage_capacity = 32};
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    template <typename tp_this>
    constexpr auto archetype_ct<tp_components...>::configuration(this tp_this&& a_this) -> decltype(auto) {
      return std::forward_like<decltype(a_this)>(a_this.m_configuration);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    archetype_ct<tp_components...>::archetype_ct(const configuration_st& a_configuration)
    : m_entities {},
      m_components {},
      m_configuration {a_configuration} {
      m_entities.reserve(m_configuration.m_component_storage_capacity);
      auto& [... components] = m_components;
      (components.reserve(m_configuration.m_component_storage_capacity), ...);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    constexpr auto archetype_ct<tp_components...>::hash() const -> archetype_hash_t {
      return archetype_hash<tp_components...>();
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto archetype_ct<tp_components...>::insert_entity(const entity_t a_entity, tp_components&&... a_components) -> void {
      m_entities.emplace_back(a_entity);

      auto& [... components] = m_components;
      (components.emplace_back(std::forward<tp_components>(a_components)), ...);
    }
  }
}
