#pragma once

import mwc_definition;
import mwc_subsystem;
import mwc_directory;

import std;

namespace mwc {
  namespace filesystem {
    struct file_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;
      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline array_t<filepath_t, std::to_underlying(directory_et::end)> directory_paths;
    };
  }
}