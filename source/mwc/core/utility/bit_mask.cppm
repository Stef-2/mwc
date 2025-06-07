module;

export module mwc_bit_mask;

import mwc_concept;

import std;

export namespace mwc {
  template <concepts::enumerator_c tp_enumerator>
  using bit_mask_t = decltype(std::to_underlying(std::declval<tp_enumerator>()));
}