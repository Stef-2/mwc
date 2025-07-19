module;

export module mwc_compiler;

export namespace mwc {
  namespace utility {
    enum class compiler_et { e_gcc, e_clang };

    constexpr auto compiler() -> compiler_et {
#ifdef __GNUC__
      return compiler_et::e_gcc;
#endif
#ifdef __clang__
      return compiler_et::e_clang;
#endif
    }
  }
}
