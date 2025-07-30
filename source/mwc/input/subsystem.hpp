#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/window/subsystem.hpp"

import mwc_definition;
import mwc_subsystem;

import fastgltf;

import std;

namespace mwc {
  namespace input {
    struct input_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline fastgltf::Parser gltf_parser;
    };

    namespace global {
      inline auto input_subsystem =
        input_subsystem_st {{&diagnostic::log::global::logging_subsystem, &mwc::global::window_subsystem},
                            string_view_t {"input subsystem"}};
    }
  }
}
