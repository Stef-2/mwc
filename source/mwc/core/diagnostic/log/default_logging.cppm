module;

export module mwc_default_logging;

import mwc_definition;

export namespace mwc {
  namespace diagnostic {
    // default logging enabled
    consteval auto default_logging() {
      return bool {MWC_LOG};
    }

    consteval auto default_logging_directory() {
      return string_view_t {MWC_LOG_DIR};
    }
  }
}