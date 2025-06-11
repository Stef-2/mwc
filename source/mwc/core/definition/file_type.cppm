module;

export module mwc_file_type;

export namespace mwc {
  enum class file_type_et { e_binary, e_text };

  template <file_type_et tp_file_type>
  consteval auto file_type_extension() {
    switch (tp_file_type) {
      case file_type_et::e_binary : return ".bin";
      case file_type_et::e_text : return ".txt";
    }
  }
}