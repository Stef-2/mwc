module;

#include "mwc/core/contract/definition.hpp"

export module mwc_directory;

import mwc_definition;
import mwc_project_name_string;

import std;

export namespace mwc {
  namespace filesystem {
    enum class directory_et { e_root, e_source, e_data, e_log, e_current_working, e_temporary, end };

    constexpr auto directory_type_string(directory_et a_directory) -> const string_view_t {
      using enum directory_et;
      switch (a_directory) {
        case e_root : return project_name_string();
        case e_source : return "source";
        case e_data : return "data";
        case e_log : return "log";
        case e_current_working : return "current working";
        case e_temporary : return "temporary";
        default : contract_assert(false); std::unreachable();
      }
    }
  }
}