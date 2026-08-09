module;

export module mwc_enumerator_reflection;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  constexpr auto enumerator_name_string(const concepts::enumerator_c auto a_enumerator) -> string_view_t pre(validate_enumerator(a_enumerator)) {
    return std::meta::identifier_of(^^a_enumerator);
  }
}