module;

export module mtr_bit;

import mtr_definition;

import std;

export namespace mtr
{
  namespace utility
  {
    template <size_t b>
    consteval auto bit()
    {
      if constexpr (b < std::numeric_limits<uint8_t>::digits)
        return uint8_t {1} << b;
      else if constexpr (b < std::numeric_limits<uint16_t>::digits)
        return uint16_t {1} << b;
      else if constexpr (b < std::numeric_limits<uint32_t>::digits)
        return uint32_t {1} << b;
      else if constexpr (b < std::numeric_limits<uint64_t>::digits)
        return uint64_t {1} << b;
      else
        static_assert(false, "bit shift out of range");
    }

  }
}