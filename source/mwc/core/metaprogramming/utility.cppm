module;

export module mwc_metaprogramming_utility;

import mwc_definition;

import std;

export namespace mwc {
  template <typename tp_specialization, template <typename...> typename tp>
  constexpr inline bool_t specialization_of_v = std::false_type {};

  // assert that [tp_specialization] is a specialization of [tp]
  template <template <typename...> typename tp_specialization, typename... tp>
  constexpr inline bool_t specialization_of_v<tp_specialization<tp...>, tp_specialization> = std::true_type {};

  // constant evaluated assertion that the number of [tp] template parameters is within [tp_min] and [tp_max]
  template <size_t tp_min, size_t tp_max, typename... tps>
  consteval auto within_bounds() {
    constexpr auto check = sizeof...(tps) >= tp_min and sizeof...(tps) <= tp_max;

    return check;
  }

  // constant evaluated for-loop
  template <size_t tp_i, size_t tp_n, typename tp_functor>
  constexpr auto static_for_loop(const tp_functor& a_functor) {
    static_assert(tp_i <= tp_n);
    if constexpr (tp_i < tp_n) {
      a_functor.template operator()<tp_i>();
      static_for_loop<tp_i + 1, tp_n>(a_functor);
    }
  }
}