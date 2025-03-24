module;

export module mtr_log;

import std;

export namespace mtr
{
    namespace log
    {
        

        enum class severity : std::uint8_t
        {
            e_information = 1 << 0,
            e_warning = 1 << 1,
            e_error = 1 << 2,
            e_critical = 1 << 3
        };
    }
}