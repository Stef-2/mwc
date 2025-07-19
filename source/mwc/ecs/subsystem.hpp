#pragma once

#include "mwc/core/diagnostic/log/subsystem.hpp"

import mwc_definition;
import mwc_subsystem;

namespace mwc {
  namespace ecs {
    struct ecs_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;
    };
  }

  namespace global {
    inline auto ecs_subsystem =
      ecs::ecs_subsystem_st {{&diagnostic::log::global::logging_subsystem}, string_view_t {"entity component subsystem"}};
  }
}
