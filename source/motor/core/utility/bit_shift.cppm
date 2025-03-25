module;

export module mtr_bit_shift;

import std;

export namespace mtr
{
    namespace utility
    {
        /*template <std::size_t N>
        using type = std::conditional_t<(N < std::numeric_limits<std::uint8_t>::digits), std::uint8_t,
            std::conditional_t<(N < std::numeric_limits<std::uint16_t>::digits), std::uint16_t,
            std::conditional_t<(N < std::numeric_limits<std::uint32_t>::digits), std::uint32_t,
            std::conditional_t<(N < std::numeric_limits<std::uint64_t>::digits), std::uint64_t,
            std::uintmax_t>>>>;*/

        //template <std::size_t N = std::numeric_limits<std::size_t>::max()>
        template <std::size_t N>
        constexpr auto bit(/*auto N = std::numeric_limits<std::size_t>::max()*/)
        {
            using type = std::conditional_t<(N < std::numeric_limits<std::uint8_t>::digits), std::uint8_t,
            std::conditional_t<(N < std::numeric_limits<std::uint16_t>::digits), std::uint16_t,
            std::conditional_t<(N < std::numeric_limits<std::uint32_t>::digits), std::uint32_t,
            std::conditional_t<(N < std::numeric_limits<std::uint64_t>::digits), std::uint64_t,
            std::uintmax_t>>>>;
            
            return type(1) << N;
            /*if consteval {
            constexpr auto WTF = N;
            if constexpr (N < std::numeric_limits<std::uint8_t>::digits)
                return static_cast<std::uint8_t>(1 << N);
            if  (N < std::numeric_limits<std::uint16_t>::digits)
                return static_cast<std::uint16_t>(1 << N);
            if  (N < std::numeric_limits<std::uint32_t>::digits)
            return static_cast<std::uint32_t>(1 << N);
            if  (N < std::numeric_limits<std::uint64_t>::digits)
            return static_cast<std::uint64_t>(1 << N);
            }*/
            //else {return 4;}
            /*if consteval {
            using x = std::conditional_t<(N < std::numeric_limits<std::uint8_t>::digits), std::uint8_t,
                std::conditional_t<(N < std::numeric_limits<std::uint16_t>::digits), std::uint16_t,
                std::conditional_t<(N < std::numeric_limits<std::uint32_t>::digits), std::uint32_t,
                std::conditional_t<(N < std::numeric_limits<std::uint64_t>::digits), std::uint64_t,
                std::uintmax_t>>>>;
            return x(1) << N;
            }*/
        }

        // generate a bit mask for a given bit position
        // return the smallest possible integral type capable of holding the bit mask
        constexpr auto bit_mask(const std::size_t a_bit_position)
        {
            return 1 << a_bit_position;
        }

        /*constexpr auto test(auto x)
        {
            if constexpr(std::is_constant_evaluated())
            {
                return bit<x>();
            }
            else
            {
                return bit_mask(x);
            }
        }*/

        consteval void wtf()
        {
            //constexpr int x = 3;
            //auto x = bit_mask(3);
            constexpr auto y = bit<3>();
            //auto z = test(4);
            //constexpr auto y = bit<std::uint8_t{3}>();
            //auto z = bit<x>();
        }
    }
}

//inline constexpr auto wtf = mtr::utility::bit<1>(1);