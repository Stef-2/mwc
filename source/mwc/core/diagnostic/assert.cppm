module;

export module mwc_assert;

import mwc_definition;
import mwc_debug;
import mwc_concept;

import std;

export namespace mwc
{
  namespace diagnostic
  {
    inline constexpr auto assert(const concepts::boolean_c auto a_condition,
                                 const string_view_t a_message = {}) -> void
    {
      if constexpr (not debugging())
        [[assume(a_condition)]];

      else if (not a_condition)
      {
        if (not a_message.empty())
          std::print("{0}\n", a_message);

        std::abort();
      }
    }
  }
}