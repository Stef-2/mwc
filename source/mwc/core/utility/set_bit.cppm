module;

export module mwc_set_bit;

import mwc_definition;

import std;

export namespace mwc {
  namespace utility {
    // generate a bit sequence with only [tp_bit] set to 1
    // packed into the smallest possible unsigned integral type capable of holding it
    template <size_t tp_bit>
    consteval auto set_bit() {
      if constexpr (tp_bit < std::numeric_limits<uint8_t>::digits)
        return uint8_t {1} << (tp_bit - 1);
      else if constexpr (tp_bit < std::numeric_limits<uint16_t>::digits)
        return uint16_t {1} << (tp_bit - 1);
      else if constexpr (tp_bit < std::numeric_limits<uint32_t>::digits)
        return uint32_t {1} << (tp_bit - 1);
      else if constexpr (tp_bit < std::numeric_limits<uint64_t>::digits)
        return uint64_t {1} << (tp_bit - 1);
      else
        static_assert(false, "bit shift out of range");
    }
  }
}