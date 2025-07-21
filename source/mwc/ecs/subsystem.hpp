#pragma once

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_ecs_definition;
import mwc_observer_ptr;

namespace mwc {
  namespace ecs {
    struct ecs_subsystem_st : public subsystem_st {
      struct archetype_storage_st {
        archetype_base_st m_archetype_base;
        vector_t<component_index_t> m_component_indices;
      };
      using archetype_storage_t = vector_t<archetype_storage_st>;

      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline auto archetypes = archetype_storage_t {};
    };

    template <size_t tp_component_count>
    [[nodiscard]] auto generate_archetype() -> archetype_ct<tp_component_count> {}
    template <typename... tp_components>
    [[nodiscard]] auto generate_entity() -> entity_t {
      constexpr auto component_indices = array_t<component_index_t, sizeof...(tp_components)> {tp_components::identity...};
      for (const auto& archetype : ecs_subsystem_st::archetypes)
        if (std::ranges::includes(archetype.m_component_indices, component_indices))
          ;
      ;
    }
    // implementation
    /*template <typename... tp_components>
    auto ecs_subsystem_st::generate_archetype() {
      for (const auto archetype : archetypes) {
        if ()
      }
    }*/
  }

  namespace global {
    inline auto ecs_subsystem =
      ecs::ecs_subsystem_st {{&diagnostic::log::global::logging_subsystem}, string_view_t {"entity component subsystem"}};
  }
}
