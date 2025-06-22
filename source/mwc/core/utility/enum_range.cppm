module;

export module mwc_enum_range;

import mwc_concept;

import std;

export namespace mwc {
  namespace utility {
    template <concepts::steady_monotonic_enum_c tp>
    constexpr auto enum_range() {
      return std::views::iota(0, std::to_underlying(tp::end));
    }
  }
}