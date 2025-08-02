module;

export module mwc_hash;

import mwc_definition;
import mwc_static_string;

export namespace mwc {
  constexpr auto polynomial_rolling_hash(const span_t<const char_t> a_string) -> size_t {
    // polynomial rolling hash
    constexpr auto prime = size_t {53};
    constexpr auto modulus = size_t {55009};

    auto hash = size_t {0};
    auto power = size_t {1};

    for (auto i = 0u; i < a_string.size(); ++i) {
      hash = (hash + (a_string[i] - 'a' + 1) * power) % modulus;
      power = (power * prime) % modulus;
    }

    return hash;
  }
}