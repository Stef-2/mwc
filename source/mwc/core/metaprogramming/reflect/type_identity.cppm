module;

export module mwc_type_identity;

import mwc_definition;
import mwc_stateful;
import mwc_compiler;

import std;

export namespace mwc {
  // crtp type to be inherited by types requiring constant evaluated name reflection and unique type hash generation
  // built on constant evaluated reflection of implementation defined output of [std::source_location::function_name()]
  template <typename tp>
  // only works on clang
    requires(utility::compiler() == utility::compiler_et::e_clang)
  struct type_name_st {
    static consteval auto type_name(const bool include_namespace = true) {
      constexpr auto source_location = std::source_location::current();
      constexpr auto function_name = std::string_view {source_location.function_name()};

      constexpr auto last_open_angle_bracket_position = function_name.rfind('<');
      constexpr auto last_closed_angle_bracket_position = function_name.rfind('>');

      if (include_namespace) {
        constexpr auto begin = last_open_angle_bracket_position + 1;
        constexpr auto size = last_closed_angle_bracket_position - begin;

        return function_name.substr(begin, size);
      } else {
        constexpr auto scope_resolution_operator = std::string_view {"::"};
        constexpr auto begin =
          function_name.rfind(scope_resolution_operator, last_closed_angle_bracket_position) + scope_resolution_operator.size();
        constexpr auto size = last_closed_angle_bracket_position - begin;

        return function_name.substr(begin, size);
      }
    }
    static consteval auto type_hash() {
      constexpr auto name = type_name(/*include_namespace*/ true);

      // polynomial rolling hash
      constexpr auto prime = std::size_t {53};
      constexpr auto modulus = std::size_t {55009};

      auto hash = std::size_t {0};
      auto power = std::size_t {1};

      for (const auto character : name) {
        hash = (hash + (character - 'a' + 1) * power) % modulus;
        power = (power * prime) % modulus;
      }

      return hash;
    }
  };

  // separate type identity function built on stateful metaprogramming
  // [tp_identity] is passed to make the compiler believe that the identity is dependent on it
  // it is substituted with [void] instead, forcing the type counter to always increment
  template <typename tp_identity, auto tp_state = 0, auto = [] {}>
  [[nodiscard]] consteval auto type_identity() -> decltype(tp_state) {
    if constexpr (requires { function(type_identified_declaration_st<void, tp_state> {}); })
      return type_identity<void, tp_state + 1>();
    else
      std::ignore = type_identified_definition_st<void, tp_state> {};
    return tp_state;
  }

  // crtp type to be inherited by types requiring unique identification
  template <typename tp, typename tp_size = uint64_t>
  struct type_identity_st {
    static constexpr auto identity = tp_size {mwc::type_identity<tp, tp_size {0}>()};
  };
}