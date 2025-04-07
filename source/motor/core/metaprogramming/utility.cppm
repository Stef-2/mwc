module;

export module mwc_metaprogramming_utility;

import mwc_definition;

import std;

export namespace mwc
{
  template <class T, template <class...> class U>
  constexpr inline bool instance_of_v = std::false_type {};

  template <template <class...> class U, class... Vs>
  constexpr inline bool instance_of_v<U<Vs...>, U> = std::true_type {};

  template <size_t tp_min, size_t tp_max, typename... t>
  consteval auto within_bounds()
  {
    constexpr auto check = sizeof...(t) >= tp_min and sizeof...(t) <= tp_max;

    return check;
  }

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

  //constexpr auto test = static_for_loop<0, 10>([&]<size_t i>() {
}