module;

export module mtr_sink;

import mtr_bit_shift;

import std;

export namespace mtr
{
    namespace log
    {
        class sink
        {
        public:
            enum class type : std::uint8_t
            {
                e_console = bit(1),
                e_string = bit(2),
                e_file = bit(3)
            };


            struct configuration
            {
                //std::bitset<sizeof(type)> m_type;
            };
        };
    }
}