#pragma once

#include "mwc/core/contract/definition.hpp"

import mwc_static_string;

namespace mwc {
  namespace reflect {
    template <static_string_st tp_string>
    consteval auto enumerator_count() {
      constexpr auto code = file_contents();
      // generate a string_view_t starting at the position of the first occurrence of [tp_string]
      constexpr auto view = string_view_t(code.begin() + symbol_offset<tp_string>(), code.end());

      // parse the substring
      for (auto count = 1uz; const auto character : view) {
        // end of the enum definition reached, return
        if (character == ';')
          return count;

        // increment counter for each defined enumerator
        if (character == ',')
          ++count;
      }
    }
  }
}
