#pragma once

#include "mwc/ecs/definition.hpp"
#include "mwc/ecs/subsystem.hpp"
#include "mwc/ecs/component_convergence.hpp"

import mwc_type_mobility;

namespace mwc {
  namespace ecs {
    struct entity_ct : public irreproducible_st {
      public:
      template <component_c... tp_components>
      constexpr entity_ct(tp_components&&... a_components);
      constexpr entity_ct(entity_ct&&) noexcept;
      constexpr auto operator=(entity_ct&&) noexcept -> entity_ct&;
      constexpr ~entity_ct();

      constexpr auto operator<=>(const entity_ct& a_other) const;
      constexpr operator entity_index_t() const;

      constexpr auto index() const -> entity_index_t;
      template <component_c... tp_components>
      constexpr auto components() const;
      template <component_c... tp_components>
      constexpr auto insert_components(tp_components&&... a_components) const;
      template <component_c... tp_components>
      constexpr auto remove_components() const;

      private:
      entity_index_t m_index;
    };

    // implementation
    template <component_c... tp_components>
    constexpr entity_ct::entity_ct(tp_components&&... a_components)
    : m_index {mwc::ecs::generate_entity(std::forward<tp_components>(a_components)...)} {}

    template <component_c... tp_components>
    constexpr auto entity_ct::components() const {
      return entity_components<tp_components...>(m_index);
    }
    template <component_c... tp_components>
    constexpr auto entity_ct::insert_components(tp_components&&... a_components) const {
      ecs::insert_components<tp_components...>(m_index, std::forward<tp_components>(a_components)...);
    }
    template <component_c... tp_components>
    constexpr auto entity_ct::remove_components() const {
      ecs::remove_components<tp_components...>(m_index);
    }
  }
}