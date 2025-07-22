#pragma once

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_ecs_definition;
import mwc_ecs_component;
import mwc_observer_ptr;

namespace mwc {
  namespace ecs {
    struct ecs_subsystem_st : public subsystem_st {
      struct archetype_storage_st {
        //archetype_base_st m_archetype_base;
        vector_t<component_index_t> m_component_indices;
      };
      using archetype_base_ptr_t = std::unique_ptr<archetype_base_st>;
      using archetype_storage_t = vector_t<archetype_base_ptr_t>;

      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline auto entity_count = entity_t {0};
      static inline auto archetypes = archetype_storage_t {};
    };

    template <component_c... tp_components>
    auto generate_archetype() -> void {
      ecs_subsystem_st::archetypes.emplace_back(std::make_unique<archetype_ct<tp_components...>>());
    }

    template <component_c... tp_components>
    [[nodiscard]] auto generate_entity(tp_components&&... a_components) -> entity_t {
      constexpr auto hash = archetype_hash<tp_components...>();
      const auto entity = ecs_subsystem_st::entity_count;
      ++ecs_subsystem_st::entity_count;

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

      archetype->insert_entity(entity, std::forward<tp_components>(a_components)...);

      return entity;
    }

    namespace global {
      inline auto ecs_subsystem =
        ecs::ecs_subsystem_st {{&diagnostic::log::global::logging_subsystem}, string_view_t {"entity component subsystem"}};
    }
  }
}
