#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/core/chrono/subsystem.hpp"
#include "mwc/core/container/static_bi_map.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_directory;
import mwc_enum_range;
import mwc_contract_assertion;
import mwc_project_name_string;

import std;

namespace mwc {
  namespace filesystem {
    struct file_subsystem_st : public subsystem_st {
      static constexpr auto directory_track_count = std::to_underlying(directory_et::end);

      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static inline static_unordered_bi_map_st<directory_et, file_path_t, directory_track_count> directory_map;
    };

    auto directory(const directory_et a_directory) -> const file_path_t& pre(a_directory != directory_et::end);
    auto directory(const string_view_t a_directory) -> file_path_t pre(contract::validate_storage(a_directory));
    auto last_write_time(const file_path_t& a_filepath) -> time_point_t pre(std::filesystem::exists(a_filepath));

    namespace global {
      inline auto file_subsystem
        = file_subsystem_st {{&diagnostic::log::global::logging_subsystem, &chrono::global::chrono_subsystem}, "file subsystem"};
    }
  }
}