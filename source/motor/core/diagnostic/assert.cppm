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
    constexpr auto assert(const concepts::boolean_c auto a_condition,
                          const string_view_t a_message = {}) -> void
    {
      if constexpr (not debugging())
        return;

      if (not a_condition)
      {
        if (not a_message.empty())
          std::print("{}\n", a_message);

        std::abort();
      }
    }
  }
}