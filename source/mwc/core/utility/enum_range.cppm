module;

export module mwc_enum_range;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  namespace utility {
    template <concepts::steady_monotonic_enum_c tp>
    constexpr auto enum_range() {
      return std::views::iota(0, std::to_underlying(tp::end));
    }
    template <typename tp_bit_mask_enumerator>
    constexpr auto bit_mask_enum_range() {
      constexpr auto last_bit = size_t {std::to_underlying(tp_bit_mask_enumerator::end) - 1};
      auto bit_array = array_t<std::underlying_type_t<tp_bit_mask_enumerator>, std::bit_width(last_bit)> {};

      for (auto i = size_t {0}; i < bit_array.size(); ++i)
        bit_array[i] = 1 << i;

      return bit_array;
    }
  }
}