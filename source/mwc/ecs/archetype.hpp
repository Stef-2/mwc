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

    struct archetype_base_st {
      using data_column_t = entity_t;
      using component_ptr_vector_t = vector_t<obs_ptr_t<void>>;
      using entity_storage_t = vector_t<entity_t>;
      using entity_span_t = span_t<const entity_t>;

      virtual ~archetype_base_st() = default;

      [[nodiscard]] virtual constexpr auto component_count() -> component_index_t = 0;
      [[nodiscard]] virtual constexpr auto component_indices() -> span_t<const component_index_t> = 0;
      [[nodiscard]] virtual constexpr auto hash() const -> archetype_hash_t = 0;
      [[nodiscard]] virtual auto component_column_data_pointers(const data_column_t a_component_column)
        -> component_ptr_vector_t = 0;
      virtual auto insert_component_empty_component_column() -> void = 0;
      virtual auto remove_component_column(const data_column_t a_component_column) -> void = 0;

      entity_storage_t m_entities;
    };

    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    class archetype_ct : public archetype_base_st, public irreproducible_st {
      public:
      using component_storage_t = tuple_t<vector_t<tp_components>...>;

      static constexpr auto archetype_indices = array_t<component_index_t, sizeof...(tp_components)> {tp_components::identity...};

      struct configuration_st {
        static constexpr auto default_configuration() -> configuration_st;

        size_t m_initial_storage_capacity;
      };

      archetype_ct(const configuration_st& a_configuration = configuration_st::default_configuration());

      [[nodiscard]] constexpr auto component_count() -> component_index_t override final;
      [[nodiscard]] constexpr auto component_indices() -> span_t<const component_index_t> override final;
      [[nodiscard]] constexpr auto hash() const -> archetype_hash_t override final;
      [[nodiscard]] auto component_column_data_pointers(const data_column_t a_component_column)
        -> component_ptr_vector_t override final;
      auto insert_component_column(tp_components&&... a_components) -> void;
      auto remove_component_column(const data_column_t a_component_column) -> void override final;
      auto insert_component_empty_component_column() -> void override final;

      template <typename tp_this>
      [[nodiscard]] constexpr auto configuration(this tp_this&& a_this) -> decltype(auto);

      //private:
      component_storage_t m_components;
      configuration_st m_configuration;
    };

    // implementation
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    constexpr auto archetype_ct<tp_components...>::configuration_st::default_configuration() -> configuration_st {
      return configuration_st {.m_initial_storage_capacity = 32};
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
    : m_components {},
      m_configuration {a_configuration} {
      m_entities.reserve(m_configuration.m_initial_storage_capacity);
      auto& [... components] = m_components;
      (components.reserve(m_configuration.m_initial_storage_capacity), ...);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    constexpr auto archetype_ct<tp_components...>::component_count() -> component_index_t {
      return sizeof...(tp_components);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    constexpr auto archetype_ct<tp_components...>::component_indices() -> span_t<const component_index_t> {
      return {archetype_indices};
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    constexpr auto archetype_ct<tp_components...>::hash() const -> archetype_hash_t {
      return archetype_hash<tp_components...>();
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto archetype_ct<tp_components...>::insert_component_column(tp_components&&... a_components) -> void {
      auto& [... components] = m_components;

      (components.emplace_back((std::forward<tp_components>(a_components))), ...);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto archetype_ct<tp_components...>::remove_component_column(const data_column_t a_component_column) -> void {
      auto& [... components] = m_components;

      m_entities.erase(m_entities.begin() + a_component_column);
      (components.erase(components.begin() + a_component_column), ...);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto archetype_ct<tp_components...>::insert_component_empty_component_column() -> void {
      auto& [... components] = m_components;

      (components.resize(components.size() + 1), ...);
    }
    template <component_c... tp_components>
      requires(sizeof...(tp_components) > 0)
    auto archetype_ct<tp_components...>::component_column_data_pointers(const data_column_t a_component_column)
      -> component_ptr_vector_t {
      auto& [... components] = m_components;

      return vector_t {(static_cast<obs_ptr_t<void>>(&components[a_component_column]), ...)};
    }
  }
}