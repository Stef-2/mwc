module;

export module mwc_memory_conversion;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  template <concepts::ratio_c tp_ratio>
  constexpr auto byte_count(size_t a_memory) {
    return a_memory * tp_ratio::num / tp_ratio::den;
  }
}
