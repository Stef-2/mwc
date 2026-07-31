module;

export module mwc_type_identity;

import mwc_definition;
import mwc_static_array;
import mwc_sso_capacity;
import mwc_hash;

import std;

export namespace mwc {
  namespace meta {
    // crtp type to be inherited by types requiring constant evaluated name reflection and unique type hash generation
    // built on constant evaluated reflection of implementation defined output of [std::source_location::function_name()]
    template <typename tp>
    struct type_name_identity_st {
      static constexpr auto type_name(const bool a_include_namespace = true) {
        constexpr auto type_name = ^^tp;

        return std::meta::identifier_of(type_name);
      }
      static constexpr auto type_name_hash() {
        constexpr auto name = type_name(/*include_namespace*/ true);

        return polynomial_rolling_hash(span_t<const char_t> {name.data(), name.size()});
      }
    };

    // crtp type to be inherited by types requiring unique identification via type name hashing
    template <typename tp, typename tp_size = size_t>
    struct type_name_hash_st {
      using value_t = tp_size;
      static constexpr auto index = value_t {type_name_identity_st<tp>::type_name_hash()};
    };

    // crtp type to be inherited by types requiring unique, steady, monotonic indices
    // forward declaration
    template <typename tp>
    struct type_index_st;

    // index generator
    template <typename tp>
    constexpr auto index() -> size_t {
      static constexpr auto descendents = search([](const std::meta::info a_info) consteval -> bool {
        return std::meta::is_type(a_info)
           and std::meta::is_base_of_type(std::meta::substitute(^^type_index_st,
                                                                {
                                                                a_info}),
                                          a_info);
      });
      static constexpr auto descendent_static_array = static_array_st(descendents);
      constexpr auto descendent_index = std::ranges::find(descendent_static_array.m_data, ^^tp);

      return std::distance(descendent_static_array.m_data.begin(), descendent_index);
    }
    // definition
    template <typename tp>
    struct type_index_st {
      static constexpr auto index = size_t {mwc::meta::index<tp>()};
    };
  }
}