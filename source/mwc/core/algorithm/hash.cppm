module;

export module mwc_hash;

import mwc_definition;
import mwc_static_string;

export namespace mwc {
  template <static_string_st tp_static_string>
  consteval auto hash() -> size_t {
    // polynomial rolling hash
    constexpr auto prime = size_t {53};
    constexpr auto modulus = size_t {55009};

    auto hash = size_t {0};
    auto power = size_t {1};

    for (const auto character : string_view_t {tp_static_string}) {
      hash = (hash + (character - 'a' + 1) * power) % modulus;
      power = (power * prime) % modulus;
    }

    return hash;
  }
}