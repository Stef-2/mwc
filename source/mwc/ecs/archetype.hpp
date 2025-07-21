#pragma once

import mwc_definition;
import mwc_ecs_definition;
import mwc_type_mobility;
import mwc_observer_ptr;

import std;

namespace mwc {
  namespace ecs {
    struct archetype_base_st {};

    template <size_t tp_component_count>
      requires(tp_component_count > 0)
    class archetype_ct : public archetype_base_st, public immobile_st {
      public:
      struct component_storage_st {
        component_index_t m_index;
        vector_t<obs_ptr_t<void>> m_data;
      };

      using entity_storage_t = vector_t<entity_t>;
      using component_storage_t = array_t<component_storage_st, tp_component_count>;

      struct configuration_st {
        static constexpr auto default_configuration() -> configuration_st;

        size_t m_entity_storage_capacity;
        size_t m_component_storage_capacity;
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
    template <size_t tp_component_count>
      requires(tp_component_count > 0)
    constexpr auto archetype_ct<tp_component_count>::configuration_st::default_configuration() -> configuration_st {
      return configuration_st {.m_entity_storage_capacity = 32, .m_component_storage_capacity = 4};
    }
    template <size_t tp_component_count>
      requires(tp_component_count > 0)
    template <typename tp_this>
    constexpr auto archetype_ct<tp_component_count>::configuration(this tp_this&& a_this) -> decltype(auto) {
      return std::forward_like<decltype(a_this)>(a_this.m_configuration);
    }
    template <size_t tp_component_count>
      requires(tp_component_count > 0)
    archetype_ct<tp_component_count>::archetype_ct(const configuration_st& a_configuration)
    : m_entities {},
      m_components {},
      m_configuration {a_configuration} {
      m_entities.reserve(m_configuration.m_entity_storage_capacity);
      for (auto& component : m_components)
        component.m_data.reserve(m_configuration.m_component_storage_capacity);
    }
  }
}
