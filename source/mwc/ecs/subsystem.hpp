#pragma once

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/ecs/archetype.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_observer_ptr;

namespace mwc {
  namespace ecs {
    struct ecs_subsystem_st : public subsystem_st {
      using archetype_storage_t = vector_t<obs_ptr_t<void>>;
      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      template <typename... tp_components>
      auto generate_archetype();

      static inline auto archetypes = archetype_storage_t {};
    };

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
