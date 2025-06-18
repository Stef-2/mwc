module;

export module mwc_directory;

import mwc_definition;

import std;

export namespace mwc {
  namespace filesystem {
    enum class directory_et { e_root, e_source, e_data, e_log, end };

    template <directory_et tp_directory>
    consteval auto directory_type_string() {
      using enum directory_et;
      switch (tp_directory) {
        case e_root : return "mwc";
        case e_source : return "source";
        case e_data : return "data";
        case e_log : return "log";
        default : contract_assert(false); std::unreachable();
      }
    }
  }
}