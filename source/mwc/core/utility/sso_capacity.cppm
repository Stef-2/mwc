module;

export module mwc_sso_capacity;

import mwc_definition;

export namespace mwc {
  namespace utility {
    consteval auto sso_capacity() {
      string_t string;

      return string.capacity();
    }
  }
}
