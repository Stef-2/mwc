#include "mwc/core/filesystem/subsystem.hpp"

import mwc_memory_conversion;

namespace mwc {
  namespace filesystem {
    auto file_subsystem_st::initialize() -> void {
      // discover and assign core directories
      constexpr auto core_directory_count = 3;
      directory_map.m_storage[0] = {directory_et::e_current_working, std::filesystem::current_path()};
      directory_map.m_storage[1] = {directory_et::e_temporary, std::filesystem::temp_directory_path()};
      directory_map.m_storage[2] = {directory_et::e_root, std::invoke([] {
                                      auto cwd = std::filesystem::current_path();
                                      while (true) {
                                        if (string_view_t {cwd.filename().c_str()} == project_name_string())
                                          return cwd;
                                        cwd = cwd.parent_path();
                                      }
                                      contract_assert(false);
                                      std::unreachable();
                                    })};

      // current filesystem space usage
      const auto filesystem_space = std::filesystem::space(directory_map[directory_et::e_root]);
      constexpr auto bytes_in_gibibyte = byte_count<mwc::gibi>(1);
      information(std::format("filesystem disk space usage:" SUB "capacity: {0} GiB" SUB "free: {1} GiB" SUB
                              "available: {2} GiB",
                              filesystem_space.capacity / bytes_in_gibibyte, filesystem_space.free / bytes_in_gibibyte,
                              filesystem_space.available / bytes_in_gibibyte));

      // discover and assign other project directories of interest
      for (auto i = core_directory_count; const auto directory_of_interest : utility::enum_range<directory_et>()) {
        using enum directory_et;
        for (const auto& directory : std::filesystem::recursive_directory_iterator(directory_map[e_root])) {
          const auto& path = directory.path();
          const auto directory_enumerator = directory_et {directory_of_interest};
          const auto directory_present = (directory_enumerator == e_current_working) or (directory_enumerator == e_root)
                                      or (directory_enumerator == e_temporary);
          const auto directory_name_match
            = string_view_t {path.filename().c_str()} == directory_type_string(directory_enumerator);
          if (directory_name_match and not directory_present) {
            directory_map.m_storage[i] = {directory_enumerator, path};
            ++i;
            break;
          }
        }
      }
      // print the results
      auto buffer = string_t {};
      constexpr auto average_output_line_length = string_view_t {"[0] directory: /path/to/directory"}.size();
      buffer.reserve(directory_track_count * average_output_line_length);
      for (auto i = 0; const auto& [directory, path] : directory_map.m_storage) {
        contract_assert(std::filesystem::exists(path) and std::filesystem::is_directory(path));
        std::format_to(std::back_inserter(buffer), "" SUB "[{0}] {1} directory: {2}", i, directory_type_string(directory),
                       path.string());
        ++i;
      }
      information(std::format("discovered the following project directories:{0}", buffer));

      m_initialized = true;
    }
    auto file_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
    auto directory(const directory_et a_directory) -> const file_path_t& {
      return file_subsystem_st::directory_map[a_directory];
    }
    auto directory(const string_view_t a_directory) -> file_path_t {
      for (const auto& directory_iterator : directory(directory_et::e_root))
        if (directory_iterator.filename() == a_directory)
          return directory_iterator;

      std::unreachable();
      contract_assert(false);
    }
    auto last_write_time(const file_path_t& a_filepath) -> time_point_t {
      const auto duration
        = std::chrono::duration_cast<time_point_t::duration>(std::filesystem::last_write_time(a_filepath).time_since_epoch());

      return time_point_t {} + duration;
    }
  }
}