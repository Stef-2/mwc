module;

export module mwc_file_type;

import std;

export namespace mwc {
  enum class file_type_et { e_binary, e_text, end };

  template <file_type_et tp_file_type>
  consteval auto file_type_extension_string() {
    using enum file_type_et;
    switch (tp_file_type) {
      case e_binary : return "bin";
      case e_text : return "txt";
      default : contract_assert(false); std::unreachable();
    }
  }
}