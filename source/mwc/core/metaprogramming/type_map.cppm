module;

export module mwc_type_map;

import mwc_concept;

export namespace mwc {
  // map [tp_key] to [tp_value]
  template <typename tp_key, typename tp_value>
  struct type_map_st {
    using key_t = tp_key;
    using value_t = tp_value;
  };

  // map [tp_enumerator] to [tp]
  // enum_key_value must be a constant value expression
  template <auto tp_enumerator, typename tp>
  struct enum_map_st {
    using enum_t = decltype(tp_enumerator);

    static constexpr auto s_enumerator = tp_enumerator;
    using value_t = tp;
  };
}