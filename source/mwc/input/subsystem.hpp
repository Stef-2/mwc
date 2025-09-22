#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/core/chrono/subsystem.hpp"
#include "mwc/input/data/scene.hpp"
#include "mwc/window/subsystem.hpp"

import mwc_definition;
import mwc_subsystem;

import vkfw;

import std;

namespace mwc {
  namespace input {
    struct input_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static auto poll_hardware_events() -> decltype(vkfw::pollEvents());

      struct keyboard_st {
        static inline auto key_map = unordered_set_t<vkfw::Key> {};
      };
      struct mouse_st {
        static inline auto x_position = float64_t {};
        static inline auto y_position = float64_t {};
        static inline auto key_map = unordered_set_t<vkfw::MouseButton> {};
      };
      struct filesystem_st {
        / / static inline auto resource_registry = ;
      };
    };

    auto read_text_file(const file_path_t& a_filepath) -> string_t;
    auto read_binary_file(const file_path_t& a_filepath) -> vector_t<byte_t>;
    auto read_scene_file(const file_path_t& a_filepath) -> scene_st;

    namespace global {
      inline auto input_subsystem = input_subsystem_st {
        {&diagnostic::log::global::logging_subsystem, &mwc::global::window_subsystem, &mwc::chrono::global::chrono_subsystem},
        string_view_t {"input subsystem"}};
    }
  }
}
