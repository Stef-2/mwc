#include "mwc/output/output.hpp"

namespace mwc {
  namespace output {
    auto write_text_file(const file_path_t& a_filepath, const string_view_t a_content) -> void {
      const auto file_handle = std::fopen(a_filepath.c_str(), "wb");
      contract_assert(file_handle);

      std::print(file_handle, "{0}", a_content.data());

      std::fclose(file_handle);
    }
  }
}