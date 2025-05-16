module;

export module mwc_metaprogramming_utility;

import mwc_definition;

import std;

export namespace mwc
{
  template <typename tp_specialization, template <typename...> typename tp>
  constexpr inline bool instance_of_v = std::false_type {};

  // assert that [tp_specialization] is a specialization of [tp]
  template <template <typename...> typename tp_specialization, typename... tp>
  constexpr inline bool instance_of_v<tp_specialization<tp...>, tp_specialization> = std::true_type {};

  // compile time assertion that the number of [tp] template parameters is within [tp_min] and [tp_max]
  template <size_t tp_min, size_t tp_max, typename... tp>
  consteval auto within_bounds()
  {
    constexpr auto check = sizeof...(tp) >= tp_min and sizeof...(tp) <= tp_max;

    return check;
  }

  // compile time for-loop
  template <size_t i, size_t n, typename functor>
  consteval auto static_for_loop(const functor& a_functor)
  {
    static_assert(i <= n);
    if constexpr (i < n)
    {
      a_functor.template operator()<i>();
      static_for_loop<i + 1, n>(a_functor);
    }
  }
}