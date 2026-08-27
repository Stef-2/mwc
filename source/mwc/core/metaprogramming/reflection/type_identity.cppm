module;

export module mwc_type_identity;

import mwc_definition;
import mwc_static_array;
import mwc_meta_search;
import mwc_sso_capacity;
import mwc_hash;

import std;

export namespace mwc {
  namespace meta {
    template <typename tp>
    constexpr auto type_name() -> string_view_t {
      return std::meta::identifier_of(^^tp);
    }
    template <typename tp>
    constexpr auto fully_qualified_type_name() -> string_view_t {
      auto fully_qualified_name = string_t {type_name<tp>()};

      const auto lambda = [&fully_qualified_name]<std::meta::info tp_entity>(this auto&& a_this) consteval -> void {
        if constexpr (std::meta::has_parent(tp_entity)) {
          constexpr auto parent = std::meta::parent_of(tp_entity);

          if constexpr (std::meta::has_identifier(parent)) {
            constexpr auto parent_identifier = std::meta::identifier_of(parent);

            std::format_to(std::front_inserter(fully_qualified_name), "::{0}", parent_identifier);
          }
          a_this.template operator()<parent>();
        }
      };
      lambda.template operator()<^^tp>();

      return std::define_static_string(fully_qualified_name);
    }
    // crtp type to be inherited by types requiring constant evaluated name reflection and unique type hash generation
    // built on constant evaluated reflection of implementation defined output of [std::source_location::function_name()]
    template <typename tp>
    struct type_name_identity_st {
      static constexpr auto type_name(const bool_t a_include_namespace = true) -> string_view_t {
        if (a_include_namespace) {
          return fully_qualified_type_name<tp>();
        } else {
          return mwc::meta::type_name<tp>();
        }
      }
      static constexpr auto type_name_hash() -> size_t {
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

    constexpr auto f = [](const std::meta::info a_info) consteval -> bool_t { /*
      if (std::meta::is_complete_type(a_info)) {
        const auto within_std = std::meta::has_parent(a_info) and (std::meta::parent_of(a_info) == ^^std);
        const auto substitute = std::meta::substitute(^^type_index_st,
                                                      {
                                                      a_info});

        return not within_std and std::meta::is_base_of_type(substitute, a_info);
      }

      return false;*/
                                                                              return std::meta::identifier_of(a_info)
                                                                                  == string_view_t {"mwc::meta::test0"};
    };
    //static_assert(std::is_same_v<txtx, void***>);
    // index generator
    template <typename tp>
    constexpr auto index() -> size_t {
      constexpr auto descendents = meta::search(f);
      static_assert(descendents.size() > 0,
                    "the indexing search yielded no results, ensure that there are types crtp-inheriting from [type_index_st]");
      static constexpr auto descendent_static_array = static_array_st {descendents};
      constexpr auto descendent_index = std::ranges::find(descendent_static_array.array(), ^^tp);
      // static_assert(descendent_index != descendent_static_array.array().end(), "[tp] not found in [descendent_static_array]");

      return std::distance(descendent_static_array.array().begin(), descendent_index);
    }
    // definition
    template <typename tp>
    struct type_index_st {
      static constexpr auto index = size_t {meta::index<tp>()};
      static constexpr auto indexed_types_tuple() {
        //return type_info_range_to_tuple<static_array_st {s}>();
      }
    };

    struct test0 : type_index_st<test0> {};
    struct test1 : type_index_st<test1> {};
    struct test2 : type_index_st<test2> {};

    static_assert(test0::index == 333);
    static_assert(test1::index == 333);
    static_assert(test2::index == 333);
  }
  struct test3 : ::mwc::meta::type_index_st<test3> {};
  struct test4 : ::mwc::meta::type_index_st<test4> {};
  static_assert(test3::index == 333);
  static_assert(test4::index == 333);
}