module;

export module mwc_stateful;

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
}