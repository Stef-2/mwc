module;

export module mwc_breakpoint;

export namespace mwc {
  // note: replace this once C++26 <debugging> is implemented
  auto breakpoint() {
#ifdef __clang__
    __builtin_debugtrap();
#else
    __asm__("int 3");
#endif
  }
}