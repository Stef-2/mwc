module;

export module mtr_bit_shift;

import std;

export namespace mtr
{
    namespace utility
    {
        //template <std::size_t N>
        constexpr auto bit(auto N)
        {
            if consteval {
            if  (N < std::numeric_limits<std::uint8_t>::digits)
                return std::uint8_t(1) << N;
            else if  (N < std::numeric_limits<std::uint16_t>::digits)
                return std::uint16_t(1) << N;
            else if  (N < std::numeric_limits<std::uint32_t>::digits)
                return std::uint32_t(1) << N;
            else if  (N < std::numeric_limits<std::uint64_t>::digits)
                return std::uint64_t(1) << N;
            else
                return std::uintmax_t(1) << N;}
                else {return 1uz << N;}
        }

        // generate a bit mask for a given bit position
        // return the smallest possible integral type capable of holding the bit mask
        constexpr auto bit_mask(const std::size_t a_bit_position)
        {
            return 1 << a_bit_position;
        }

        constexpr auto test(auto x)
        {
            if constexpr(std::is_constant_evaluated())
            {
                return bit<x>();
            }
            else
            {
                return bit_mask(x);
            }
        }

        void wtf()
        {
            //constexpr int x = 3;
            //auto x = bit_mask(3);
            auto y = bit(4);
            //auto z = test(4);
            //constexpr auto y = bit<std::uint8_t{3}>();
            //auto z = bit<x>();
        }
    }
}

//inline constexpr auto wtf = mtr::utility::bit<1>(1);