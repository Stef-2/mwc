module;

export module mwc_type_identity;

import mwc_definition;
import mwc_ctti;
import mwc_hash;

import std;

export namespace mwc {
  namespace ctti {
    // crtp type to be inherited by types requiring constant evaluated name reflection and unique type hash generation
    // built on constant evaluated reflection of implementation defined output of [std::source_location::function_name()]
    template <typename tp>
    struct type_identity_st {
      static constexpr auto type_name(const bool a_include_namespace = true) {
        // this can't be constexpr as gcc removes template information from the function name in that case
        const auto source_location = std::source_location::current();
        const auto function_name = std::string_view {source_location.function_name()};

        constexpr auto equals_string = string_view_t {"= "};
        constexpr auto scope_resolution_string = string_view_t {"::"};

        const auto last_equals_position = function_name.rfind(equals_string) + equals_string.size();
        const auto last_scope_resolution_operator_position =
          function_name.rfind(scope_resolution_string) + scope_resolution_string.size();
        const auto begin = a_include_namespace ? last_equals_position : last_scope_resolution_operator_position;
        const auto end = function_name.rfind(']');

        return function_name.substr(begin, end - begin);
      }
      static constexpr auto type_hash() {
        constexpr auto name = type_name(/*include_namespace*/ true);

        return polynomial_rolling_hash(span_t {name.data(), name.size()});
      }
    };

    // utility required for constant evaluated, observable and modifiable state
    template <typename, auto>
    struct type_identified_declaration_st {
      friend consteval auto state_function(type_identified_declaration_st, auto...);
    };
    template <typename tp_identity, auto tp_state>
    struct type_identified_definition_st {
      friend consteval auto state_function(type_identified_declaration_st<tp_identity, tp_state>, auto...) {}
    };
    // separate type identity function built on stateful metaprogramming
    // [tp_identity] is passed to make the compiler believe that the identity is dependent on it
    // it is substituted with [void] instead, forcing the type counter to always increment
    template <typename tp_identity, auto tp_state = 0, auto = [] {}>
    [[nodiscard]] consteval auto type_identity() -> decltype(tp_state) {
      if constexpr (requires { state_function(type_identified_declaration_st<void, tp_state> {}); })
        return type_identity<void, tp_state + 1>();
      else
        std::ignore = type_identified_definition_st<void, tp_state> {};
      return tp_state;
    }

    // crtp type to be inherited by types requiring unique identification
    template <typename tp, typename tp_size = uint64_t>
    struct type_index_st {
      static constexpr auto index = tp_size {ctti::type_identity<tp, tp_size {1}>()};
    };
  }
}