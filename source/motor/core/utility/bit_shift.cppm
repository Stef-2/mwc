module;

export module mtr_bit_shift;

import std;

export namespace mtr
{
    namespace utility
    {
        // generate a bit mask for a given bit position
        // return the smallest possible integral type that can hold the bit mask
        constexpr auto bit(const std::size_t a_bit_position)
        {
            if constexpr (a_bit_position < std::numeric_limits<std::uint8_t>::digits)
                return static_cast<std::uint8_t >(1)  << a_bit_position;
            if constexpr (a_bit_position < std::numeric_limits<std::uint16_t>::digits)
                return static_cast<std::uint16_t>(1) << a_bit_position;
            if constexpr (a_bit_position < std::numeric_limits<std::uint32_t>::digits)
                return static_cast<std::uint32_t>(1) << a_bit_position;
            if constexpr (a_bit_position < std::numeric_limits<std::uint64_t>::digits)
                return static_cast<std::uint64_t>(1) << a_bit_position;
            

        }
    }
}