module;

export module mwc_debug;

export namespace mwc {
  namespace diagnostic {
    // debugging mode
    consteval auto debugging() -> bool { return bool {1}; }
  }
}