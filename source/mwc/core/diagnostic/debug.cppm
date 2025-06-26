module;

export module mwc_debug;

import mwc_definition;

export namespace mwc {
  namespace diagnostic {

    // debugging enabled
    consteval auto debugging() {
      return bool_t {MWC_DEBUG};
    }
  }
}