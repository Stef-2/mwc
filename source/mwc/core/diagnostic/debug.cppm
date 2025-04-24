module;

export module mwc_debug;

export namespace mwc {
  namespace diagnostic {
    // debugging mode
    consteval auto debugging() { return bool {MWC_DEBUG}; }
  }
}