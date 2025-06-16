module;

export module mwc_logging_subsystem_switch;

import mwc_definition;

export namespace mwc {
  namespace diagnostic {
    // default logging enabled
    consteval auto logging_subsystem_switch() {
      return bool {MWC_LOG};
    }

    consteval auto logging_subsystem_directory() {
      return string_view_t {MWC_LOG_DIR};
    }
  }
}