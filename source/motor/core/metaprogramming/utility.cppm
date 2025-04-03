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

  // ================================
  /*template <class T, class P>
  inline constexpr bool is_like_v = std::is_same_v<T, P>;

  struct _
  {};

  template <class T>
  inline constexpr bool is_like_v<T, _> = true;

  template <template <class...> class T, class... Ts, class... Us>
    requires(sizeof...(Ts) == sizeof...(Us))
  inline constexpr bool is_like_v<T<Ts...>, T<Us...>> =
    (is_like_v<Ts, Us> && ...);

  template <class T, size_t M, class U, size_t N>
  inline constexpr bool is_like_v<std::array<T, M>, std::array<U, N>> =
    is_like_v<T, U>;

  template <class... Ps>
  struct in
  {};

  template <class T, class... Ps>
  inline constexpr bool is_like_v<T, in<Ps...>> = (is_like_v<T, Ps> || ...);*/

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