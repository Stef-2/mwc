module;

export module mtr_assert;

import mtr_definition;
import mtr_debug;
import mtr_concept;
import mtr_log;

import std;

export namespace mtr
{
    constexpr auto assert(const concepts::boolean auto a_condition, const string_view_t a_message = {""}) -> void
    {
        if constexpr (not debugging())
            return;

        if (not a_condition)
        {
            std::print("{}\n", a_message);
            std::abort();
        }
    }
}