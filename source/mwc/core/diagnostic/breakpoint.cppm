module;

export module mwc_breakpoint;

export namespace mwc {
  // note: replace this once C++26 <debugging> is implemented
  auto breakpoint() { __asm__("int 3"); }
}