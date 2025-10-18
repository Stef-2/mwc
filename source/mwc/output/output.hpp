#pragma once

#include "mwc/core/contract/definition.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/core/filesystem/subsystem.hpp"
#include "mwc/core/filesystem/file_type.hpp"

import mwc_definition;

import std;

namespace mwc {
  namespace output {
    auto write_text_file(const file_path_t& a_filepath, const string_view_t a_content) -> void;
    auto write_binary_file(const file_path_t& a_filepath, const span_t<byte_t> a_content) -> void;
  }
}
