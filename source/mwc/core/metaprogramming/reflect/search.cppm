module;

export module mwc_meta_search;

import mwc_definition;
import mwc_static_array;
import mwc_namespace;

import std;

export namespace mwc {
  namespace meta {
    // recursively search through [tp_scope] and its descendents, counting the number of entities that match [tp_predicate]
    template <typename tp_predicate, std::meta::info tp_scope = mwc_namespace>
    consteval auto search_count(const tp_predicate a_predicate) -> size_t {
      auto entity_count = size_t {0};

      const auto search_engine = [&entity_count]<tp_predicate, std::meta::info tp_local_scope>(
                                   this auto&& a_this, const tp_predicate a_predicate) consteval -> void {
        static constexpr auto members
          = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));

        template for (constexpr auto member : members) {
          constexpr auto is_complete_type = std::meta::is_complete_type(member);
          constexpr auto is_not_template = not std::meta::is_template(member);
          constexpr auto is_namespace = std::meta::is_namespace(member);
          const bool found = a_predicate(member);

          if (found) {
            ++entity_count;
          }
          if constexpr (is_namespace or (is_complete_type and is_not_template)) {
            a_this.template operator()<tp_predicate, member>(a_predicate);
          }
        }
      };

      search_engine.template operator()<tp_predicate, tp_scope>(a_predicate);

      return entity_count;
    }

    // recursively search through [tp_scope] and its descendents, returning entities that match [tp_predicate]
    template <typename tp_predicate, std::meta::info tp_scope = mwc_namespace>
    consteval auto search(const tp_predicate a_predicate) {

      auto matched_entities = std::array<std::meta::info, match_count<tp_predicate, tp_scope>(a_predicate)> {};
      size_t i = 0;
      const auto search_engine = [&matched_entities, &i]<tp_predicate, std::meta::info tp_local_scope>(
                                   this auto&& a_this, const tp_predicate a_predicate) consteval -> void {
        static constexpr auto members
          = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));

        template for (constexpr auto member : members) {
          constexpr auto is_complete_type = std::meta::is_complete_type(member);
          constexpr auto is_not_template = not std::meta::is_template(member);
          constexpr auto is_namespace = std::meta::is_namespace(member);
          const bool found = a_predicate(member);

          if (found) {
            matched_entities[i] = member;
            ++i;
          }
          if constexpr (is_namespace or (is_complete_type and is_not_template)) {
            a_this.template operator()<tp_local_scope, member>(a_predicate);
          }
        }
      };

      search_engine.template operator()<tp_predicate, tp_scope>(a_predicate);

      return std::define_static_array(matched_entities);
    }

    // utility that converts a range of type reflections into a matching tuple_t
    template <auto tp>
      requires(std::meta::template_of(^^decltype(tp)) == ^^static_array_st)
    consteval auto type_info_range_tuple() {
      constexpr auto type_accumulator = []<size_t... i>(const std::index_sequence<i...> a_index_sequence) consteval {
        static_assert((std::meta::is_type(tp.m_data[i]) and ...));

        using types = tuple_t<typename[:tp.m_data[i]:]...>;
        return types {};
      };

      return type_accumulator(std::make_index_sequence<tp.m_data.size()> {});
    }
  }
}