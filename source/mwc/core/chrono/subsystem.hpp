#pragma once

#include "mwc/core/contract/definition.hpp"

import mwc_subsystem;
import mwc_definition;

import std;

namespace mwc {
  namespace chrono {
    struct chrono_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;
      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline time_point_t initialization_time;
    };

    namespace global {
      inline auto chrono_subsystem = chrono_subsystem_st {{}, "chrono subsystem"};
    }
  }
}