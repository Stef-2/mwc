module;

export module mwc_ctti;

import std;

export namespace mwc {
  // constant evaluated, observable and modifiable state
  // type identified version
  template <typename, auto>
  struct type_identified_declaration_st {
    friend consteval auto function(type_identified_declaration_st, auto...);
  };

  template <typename tp_identity, auto tp_state>
  struct type_identified_definition_st {
    friend consteval auto function(type_identified_declaration_st<tp_identity, tp_state>, auto...) {}
  };

  template <typename tp_identity, auto tp_modifier, auto tp_state = 0, auto = [] {}>
  [[nodiscard]] consteval auto modify_state() -> decltype(tp_state) {
    if constexpr (requires { function(type_identified_declaration_st<tp_identity, tp_state> {}); })
      return modify_state<tp_identity, tp_modifier, tp_modifier(tp_state)>();
    else
      std::ignore = type_identified_definition_st<tp_identity, tp_state> {};
    return tp_state;
  }

  template <typename tp_identity, auto tp_state = 0, auto = [] {}>
  [[nodiscard]] consteval auto observe_state() -> decltype(tp_state) {
    if constexpr (requires { function(type_identified_declaration_st<tp_identity, tp_state> {}); })
      return observe_state<tp_identity, tp_state + 1>();
    else
      return tp_state;
  }

  // non type identified version
  template <auto, auto>
  struct non_type_identified_declaration_st {
    friend consteval auto function(non_type_identified_declaration_st, auto...);
  };

  template <auto tp_identity, auto tp_state>
  struct non_type_identified_definition_st {
    friend consteval auto function(non_type_identified_declaration_st<tp_identity, tp_state>, auto...) {}
  };

  template <auto tp_identity, auto tp_modifier, auto tp_state = 0, auto = [] {}>
  [[nodiscard]] consteval auto modify_state() -> decltype(tp_state) {
    if constexpr (requires { function(non_type_identified_declaration_st<tp_identity, tp_state> {}); })
      return modify_state<tp_identity, tp_modifier, tp_modifier(tp_state)>();
    else
      non_type_identified_definition_st<tp_identity, tp_state> {};
    return tp_state;
  }

  template <auto tp_identity, auto tp_state = 0, auto = [] {}>
  [[nodiscard]] consteval auto observe_state() -> decltype(tp_state) {
    if constexpr (requires { function(non_type_identified_declaration_st<tp_identity, tp_state> {}); })
      return observe_state<tp_identity, tp_state + 1>();
    else
      return tp_state;
  }

  // ================================================
  template <class...>
  struct type_list {};
  template <class...>
  struct type_list2 {};

  namespace detail {
    template <template <class...> class tp_list, auto>
    struct nth {
      auto friend get(nth);
    };
    template <template <class...> class tp_list, auto N, class T>
    struct set {
      auto friend get(nth<tp_list, N>) {
        return T {};
      }
    };
    template <template <class...> class TList, class T, class... Ts>
    auto append(TList<Ts...>) -> TList<Ts..., T>;
  } // namespace detail

  template <template <class...> class tp_list, class T, auto N = 0, auto unique = [] {}>
  consteval auto append() {
    if constexpr (requires { get(detail::nth<tp_list, N> {}); })
      append<tp_list, T, N + 1, unique>();
    else if constexpr (N == 0)
      void(detail::set<tp_list, N, tp_list<T>> {});
    else
      void(detail::set<tp_list, N, decltype(detail::append<tp_list, T>(get(detail::nth<tp_list, N - 1> {})))> {});
  }

  template <template <class...> class tp_list, auto unique = [] {}, auto N = 0>
  consteval auto get_list() {
    if constexpr (requires { get(detail::nth<tp_list, N> {}); })
      return get_list<tp_list, unique, N + 1>();
    else if constexpr (N == 0)
      return tp_list {};
    else
      return get(detail::nth<tp_list, N - 1> {});
  }

  using x = decltype(append<type_list, int>());
  using x = decltype(append<type_list, float>());
  using y = decltype(append<type_list2, char>());
  static_assert(std::is_same_v<decltype(get_list<type_list>()), decltype(get_list<type_list2>())>);
}