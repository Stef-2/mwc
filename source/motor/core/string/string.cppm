module;

export module mtr_string;

import std;

export namespace mtr
{
    namespace string
    {
        using string_t = std::string;
        using char_t = string_t::value_type;
    }
}