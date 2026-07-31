#pragma once

import mwc_definition;
import mwc_log_subsystem;
import mwc_file_subsystem;
import mwc_file_type;

import std;

namespace mwc {
  namespace output {
    auto write_file(const file_path_t& a_filpath,
                    const std::ranges::range auto& a_data,
                    const std::iostream::openmode a_open_mode = std::iostream::out | std::iostream::trunc) -> void;

    // implementation
    auto
    write_file(const file_path_t& a_filpath, const std::ranges::range auto& a_data, const std::iostream::openmode a_open_mode)
      -> void {
      auto file_stream = std::ofstream {a_filpath, a_open_mode};
      contract_assert(file_stream);

      file_stream.write(reinterpret_cast<const char_t*>(a_data.data()), a_data.size());
      file_stream.close();
    }
  }
}
