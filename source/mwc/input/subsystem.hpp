#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/window/subsystem.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_input_scene;

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

    auto read_text_file(const filepath_t& a_filepath) -> string_t;
    auto read_binary_file(const filepath_t& a_filepath) -> vector_t<byte_t>;
    auto read_scene_file(const filepath_t& a_filepath) -> scene_st;
    namespace global {
      inline auto input_subsystem =
        input_subsystem_st {{&diagnostic::log::global::logging_subsystem, &mwc::global::window_subsystem},
                            string_view_t {"input subsystem"}};
    }
  }
}
