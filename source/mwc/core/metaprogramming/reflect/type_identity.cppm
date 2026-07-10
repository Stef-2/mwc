module;

export module mwc_type_identity;

import mwc_definition;
import mwc_sso_capacity;
import mwc_hash;

import std;

export namespace mwc {
  namespace meta {
    // crtp type to be inherited by types requiring constant evaluated name reflection and unique type hash generation
    // built on constant evaluated reflection of implementation defined output of [std::source_location::function_name()]
    template <typename tp>
    struct type_identity_st {
      static constexpr auto type_name(const bool a_include_namespace = true) {
        constexpr auto type_name = ^^tp;
        
        return std::meta::identifier_of(type_name);
      }
      static constexpr auto type_hash() {
        constexpr auto name = type_name(/*include_namespace*/ true);

        return polynomial_rolling_hash(span_t<const char_t> {name.data(), name.size()});
      }
    };

    // crtp type to be inherited by types requiring unique identification
    template <typename tp, typename tp_size = size_t>
    struct type_index_st {
      static constexpr auto index = tp_size {type_identity_st<tp>::type_hash()};
    };
  }
}