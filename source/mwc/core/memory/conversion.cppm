module;

export module mwc_memory_conversion;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  // note: implement these using std::pow once it is available as constexpr
  using kibi = std::ratio<1024, 1>;
  using mibi = std::ratio_multiply<kibi, kibi>;
  using gibi = std::ratio_multiply<mibi, kibi>;
  using tebi = std::ratio_multiply<gibi, kibi>;
  using pebi = std::ratio_multiply<tebi, kibi>;
  using exbi = std::ratio_multiply<pebi, kibi>;

  template <concepts::ratio_c tp_ratio>
  constexpr auto byte_count(size_t a_memory) {
    return a_memory * tp_ratio::num / tp_ratio::den;
  }
}
