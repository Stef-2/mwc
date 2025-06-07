module;

export module mwc_enum_bitwise_operators;

import mwc_concept;

import std;

export namespace mwc {
  constexpr auto operator not(const concepts::enumerator_c auto x) {
    return static_cast<decltype(x)>(not std::to_underlying(x));
  }
  constexpr auto operator bitor(const concepts::enumerator_c auto x,
                                const concepts::enumerator_c auto y)
    requires(std::is_same_v<decltype(x), decltype(y)>) {
    return static_cast<decltype(x)>(std::to_underlying(x) bitor
                                    std::to_underlying(y));
  }
  constexpr auto operator bitand(const concepts::enumerator_c auto x,
                                 const concepts::enumerator_c auto y)
    requires(std::is_same_v<decltype(x), decltype(y)>) {
    return static_cast<decltype(x)>(std::to_underlying(x) bitand
                                    std::to_underlying(y));
  }
  constexpr auto operator xor(const concepts::enumerator_c auto x,
                              const concepts::enumerator_c auto y)
    requires(std::is_same_v<decltype(x), decltype(y)>) {
    return static_cast<decltype(x)>(std::to_underlying(x) xor
                                    std::to_underlying(y));
  }
  constexpr auto& operator|=(concepts::enumerator_c auto& x,
                             const concepts::enumerator_c auto y)
    requires(std::is_same_v<decltype(x), decltype(y)>) {
    return x bitor y;
  }
  constexpr auto& operator&=(concepts::enumerator_c auto& x,
                             const concepts::enumerator_c auto y)
    requires(std::is_same_v<decltype(x), decltype(y)>) {
    return x bitand y;
  }
  constexpr auto& operator^=(concepts::enumerator_c auto& x,
                             const concepts::enumerator_c auto y)
    requires(std::is_same_v<decltype(x), decltype(y)>) {
    return x xor y;
  }
}