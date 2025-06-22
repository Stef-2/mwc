#include "mwc/core/filesystem/subsystem.hpp"

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
      constexpr auto bytes_in_gibibyte = 1024 * 1024 * 1024;
      information(std::format("filesystem disk space usage:\ncapacity: {0} GiB\nfree: {1} GiB\navailable: {2} GiB",
                              filesystem_space.capacity / bytes_in_gibibyte, filesystem_space.free / bytes_in_gibibyte,
                              filesystem_space.available / bytes_in_gibibyte));

      // discover and assign other project directories of interest
      for (auto i = core_directory_count; const auto directory_of_interest : utility::enum_range<directory_et>()) {
        using enum directory_et;
        for (const auto& directory : std::filesystem::recursive_directory_iterator(directory_map[e_root])) {
          const auto& path = directory.path();
          const auto directory_enumerator = directory_et {directory_of_interest};
          const auto directory_present = (directory_enumerator == e_current_working) or (directory_enumerator == e_root) or
                                         (directory_enumerator == e_temporary);
          const auto directory_name_match =
            string_view_t {path.filename().c_str()} == directory_type_string(directory_enumerator);
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
        std::format_to(std::back_inserter(buffer), "\n[{0}] {1}: {2}", i, directory_type_string(directory), path.string());
        ++i;
      }
      information(std::format("discovered the following project directories:{0}", buffer));

      m_initialized = true;
    }
    auto file_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
  }
}