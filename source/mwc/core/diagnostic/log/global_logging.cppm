module;

#include "mwc/core/definition/definition.hpp"

export module mwc_global_logging;

export namespace mwc {
  namespace diagnostic {

    // global logging enabled
    consteval auto global_logging() { return bool {MWC_LOG}; }

    consteval auto logging_directory() { return string_view_t {MWC_LOG_DIR}; }
  }
}