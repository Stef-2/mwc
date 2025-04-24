module;

export module mwc_global_logging;

import mwc_definition;

export namespace mwc {
  namespace diagnostic {

    // global logging enabled
    consteval auto global_logging() { return bool {MWC_LOG}; }

    consteval auto logging_directory() { return string_view_t {MWC_LOG_DIR}; }
  }
}