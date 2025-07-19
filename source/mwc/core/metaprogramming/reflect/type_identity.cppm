module;

export module mwc_type_identity;

import mwc_definition;
import mwc_compiler;

import std;

export namespace mwc {
  // crtp type to be inherited by types requiring compile time name reflection and type hash generation
  template <typename tp>
  // only works on clang
    requires(utility::compiler() == utility::compiler_et::e_clang)
  struct type_identity_st {
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
}