#pragma once

#include "mwc/core/container/static_bi_map.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_directory;
import mwc_enum_range;
import mwc_project_name_string;

import std;

namespace mwc {
  namespace filesystem {
    struct file_subsystem_st : public subsystem_st {
      static constexpr auto directory_track_count = std::to_underlying(directory_et::end);

      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline static_unordered_bi_map_st<directory_et, filepath_t, directory_track_count> directory_map;
    };

    namespace global {
      inline auto file_subsystem = file_subsystem_st {{&diagnostic::log::global::logging_subsystem}, "file subsystem"};
    }
  }
}