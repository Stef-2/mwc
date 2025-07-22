module;

export module mwc_ctti;

import std;

// internal
namespace mwc {
  namespace ctti {
    // utility requird for constant evaluated, observable and modifiable state
    // type identified version
    template <typename, auto>
    struct type_identified_declaration_st {
      friend consteval auto state_function(type_identified_declaration_st, auto...);
    };
    template <typename tp_identity, auto tp_state>
    struct type_identified_definition_st {
      friend consteval auto state_function(type_identified_declaration_st<tp_identity, tp_state>, auto...) {}
    };
    // non type identified version
    template <auto, auto>
    struct non_type_identified_declaration_st {
      friend consteval auto state_function(non_type_identified_declaration_st, auto...);
    };
    template <auto tp_identity, auto tp_state>
    struct non_type_identified_definition_st {
      friend consteval auto state_function(non_type_identified_declaration_st<tp_identity, tp_state>, auto...) {}
    };

    // utility required for constant evaulated, observable and modifiable type lists
    template <template <typename...> typename tp_list, auto>
    struct element_st {
      auto friend observe(element_st);
    };
    template <template <typename...> typename tp_list, auto tp_state, typename tp>
    struct modify_st {
      auto friend observe(element_st<tp_list, tp_state>) {
        return tp {};
      }
    };
    template <template <typename...> typename tp_list, typename tp, typename... tps>
    auto type_list_push_back(tp_list<tps...>) -> tp_list<tps..., tp>;
  }
}

export namespace mwc {
  namespace ctti {
    // constant evaluated, observable and modifiable state
    // type identified version
    template <typename tp_identity, auto tp_modifier, auto tp_state = 0, auto = [] {}>
    [[nodiscard]] consteval auto modify_state() -> decltype(tp_state) {
      if constexpr (requires { state_function(type_identified_declaration_st<tp_identity, tp_state> {}); })
        return modify_state<tp_identity, tp_modifier, tp_modifier(tp_state)>();
      else
        std::ignore = type_identified_definition_st<tp_identity, tp_state> {};
      return tp_state;
    }
    template <typename tp_identity, auto tp_state = 0, auto = [] {}>
    [[nodiscard]] consteval auto observe_state() -> decltype(tp_state) {
      if constexpr (requires { state_function(type_identified_declaration_st<tp_identity, tp_state> {}); })
        return observe_state<tp_identity, tp_state + 1>();
      else
        return tp_state;
    }
    // non type identified version
    template <auto tp_identity, auto tp_modifier, auto tp_state = 0, auto = [] {}>
    [[nodiscard]] consteval auto modify_state() -> decltype(tp_state) {
      if constexpr (requires { state_function(non_type_identified_declaration_st<tp_identity, tp_state> {}); })
        return modify_state<tp_identity, tp_modifier, tp_modifier(tp_state)>();
      else
        non_type_identified_definition_st<tp_identity, tp_state> {};
      return tp_state;
    }
    template <auto tp_identity, auto tp_state = 0, auto = [] {}>
    [[nodiscard]] consteval auto observe_state() -> decltype(tp_state) {
      if constexpr (requires { state_function(non_type_identified_declaration_st<tp_identity, tp_state> {}); })
        return observe_state<tp_identity, tp_state + 1>();
      else
        return tp_state;
    }

    // constant evaulated, observable and modifiable type lists
    template <typename...>
    struct type_list {};
    template <typename...>
    struct type_list2 {};

    template <template <typename...> typename tp_list, typename tp, auto tp_state = 0, auto tp_unique = [] {}>
    consteval auto type_list_push_back() {
      if constexpr (requires { observe(element_st<tp_list, tp_state> {}); })
        type_list_push_back<tp_list, tp, tp_state + 1, tp_unique>();
      else if constexpr (tp_state == 0)
        void(modify_st<tp_list, tp_state, tp_list<tp>> {});
      else
        void(
          modify_st<tp_list, tp_state, decltype(type_list_push_back<tp_list, tp>(observe(element_st<tp_list, tp_state - 1> {})))> {});
    }
    template <template <typename...> typename tp_list, auto tp_state = 0, auto tp_unique = [] {}>
    consteval auto observe_type_list() {
      if constexpr (requires { observe(element_st<tp_list, tp_state> {}); })
        return observe_type_list<tp_list, tp_state + 1, tp_unique>();
      else if constexpr (tp_state == 0)
        return tp_list {};
      else
        return observe(element_st<tp_list, tp_state - 1> {});
    }
    template <template <typename...> typename tp_list, auto tp_state = 0>
    consteval auto get() {
      return observe(element_st<tp_list, tp_state> {});
    }

    using x = decltype(type_list_push_back<type_list, int>());
    using x = decltype(type_list_push_back<type_list, float>());
    using y = decltype(type_list_push_back<type_list2, char>());
    static_assert(not std::is_same_v<decltype(observe_type_list<type_list>()), decltype(observe_type_list<type_list2>())>);
  }
}