module;

export module mwc_meta_search;

import mwc_definition;
import mwc_static_array;
import mwc_static_string;
import mwc_namespace;

import std;

export namespace mwc {
  namespace meta {
    // recursively search through [tp_scope] and its recursible descendents
    // returning entities that match [a_predicate]
    template <std::meta::info tp_scope = mwc_namespace>
      requires requires { std::meta::is_namespace(tp_scope); }
    consteval auto search(/*std::predicate<const std::meta::info>*/ auto a_unary_predicate) {
      // limit recursion depth in case of loops
      constexpr auto recursion_depth_limit = size_t {32};
      // determine if [a_inner_entity] can be recursively examined for contents via [std::meta::members_of()]
      // it must be either a complete type or a namespace, with some additional checks to cover edge cases
      static constexpr auto assert_recursible_scope
        = [](const std::meta::info a_outer_entity, const std::meta::info a_inner_entity) consteval -> bool_t {
        using namespace std::meta;

        const auto inner = dealias(a_inner_entity);
        const auto outer = dealias(a_outer_entity);

        const auto inner_complete_type = is_complete_type(inner) and is_class_type(inner);
        const auto inner_namespace = is_namespace(inner);
        const auto outer_namespace = is_namespace(inner);
        // exclude std namespace from search
        const auto std_namespace = (inner_namespace and inner != ^^::std) or (outer_namespace and outer != ^^::std);
        // ensure no loops are formed
        const auto member_of_self = inner_complete_type and is_complete_type(outer) and is_same_type(inner, outer);
        const auto is_self = inner == outer;

        return (inner_namespace or inner_complete_type) and not member_of_self and not is_self and std_namespace;
      };
      // recursive search engine
      constexpr auto search_engine
        = [&assert_recursible_scope, recursion_depth_limit]<std::meta::info tp_local_scope, size_t tp_recursion_depth = 0>(
            this auto&& a_this,
            decltype(a_unary_predicate) a_predicate,
            vector_t<std::meta::info>&& a_entity_accumulator = {}) consteval -> vector_t<std::meta::info> {
        constexpr auto within_recursion_depth_limit = tp_recursion_depth <= recursion_depth_limit;
        // current scope members
        static constexpr auto members
          = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));

        template for (constexpr auto member : members) {
          // static_assert(std::is_same_v<decltype(a_predicate), void*>);
          if (a_predicate(member)) {
            a_entity_accumulator.push_back(member);
          }

          // determine if [member] can be recursed through
          constexpr auto recursible_scope = assert_recursible_scope(tp_local_scope, member);
          if constexpr (recursible_scope and within_recursion_depth_limit) {
            a_entity_accumulator
              = a_this.template operator()<member, tp_recursion_depth + 1>(a_predicate, std::move(a_entity_accumulator));
          }
        }

        return a_entity_accumulator;
      };
      // can't return this directly as the resulting span would deduce size to be [std::dynamic_extent]
      constexpr auto match = std::define_static_array(search_engine.template operator()<tp_scope>(a_unary_predicate));

      // return subspan with the actual compile time size
      return match.template subspan<0, match.size()>();
    }

    // utility that converts a range of type reflections into a matching tuple_t
    template <auto tp>
    // reflection ranges are not structural and thus can't be used as template parameters
    // they have to be wrapped in [static_array_st]
      requires(std::meta::template_of(^^decltype(tp)) == ^^static_array_st)
    consteval auto type_info_range_to_tuple() {
      constexpr auto type_accumulator
        = []<size_t... i>([[maybe_unused]] const std::index_sequence<i...> a_index_sequence) consteval {
            static_assert((std::meta::is_type(tp.m_data[i]) and ...));

            using types = tuple_t<typename[:tp.m_data[i]:]...>;
            return types {};
          };

      return type_accumulator(std::make_index_sequence<tp.m_data.size()> {});
    }

    template <auto tp>
      requires(std::meta::template_of(^^decltype(tp)) == ^^static_array_st)
    struct type_info_range_tuple_st {
      //using tuple_t = decltype(type_info_range_tuple<tp>());
    };
  }
}
