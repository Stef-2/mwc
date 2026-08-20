module;

export module mwc_meta_search;

import mwc_definition;
import mwc_static_array;
import mwc_namespace;

import std;

namespace mwc {
  namespace meta{
    consteval auto verify_recursion_candidate(const std::meta::info a_outer_entity, const std::meta::info a_inner_entity) -> std::optional<std::meta::info> {
      const auto dealiased_member = std::meta::is_type_alias(a_inner_entity) ? std::meta::dealias(a_inner_entity) : a_inner_entity;
      const auto is_complete_type = std::meta::is_complete_type(dealiased_member) and std::meta::is_class_type(dealiased_member);
      const auto is_not_template = not std::meta::is_template(dealiased_member);
      const auto is_namespace = std::meta::is_namespace(dealiased_member);
      const auto member_of_self = dealiased_member != a_outer_entity;

      if ((is_namespace or (is_complete_type and is_not_template)) and not member_of_self) {
        return a_inner_entity;
      } else {
        return std::nullopt;
      }
    }
  }
}

export namespace mwc {
  namespace meta {
    // recursively search through [tp_scope] and its descendents, counting the number of entities that match [tp_predicate]
    template <typename tp_predicate, std::meta::info tp_scope = mwc_namespace>
      requires requires {std::meta::is_namespace(tp_scope) and std::is_invocable_r_v<bool, tp_predicate, const std::meta::info>;}
    consteval auto search_count(const tp_predicate a_predicate) -> size_t {
      auto entity_count = size_t {0};

      const auto search_engine = [&entity_count]<typename tp_local_predicate, std::meta::info tp_local_scope>(
                                   this auto&& a_this, const tp_local_predicate a_predicate) consteval -> void {
        static constexpr auto members
          = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));

        template for (constexpr auto member : members) {
          constexpr auto recursion_candidate = verify_recursion_candidate(member, tp_local_scope);
          const bool found = a_predicate(member);

          if (found) {
            ++entity_count;
          }
          if constexpr (recursion_candidate.has_value()) {
            a_this.template operator()<tp_predicate, recursion_candidate.value()>(a_predicate);
          }
        }
      };

      search_engine.template operator()<tp_predicate, tp_scope>(a_predicate);

      return entity_count;
    }

    // recursively search through [tp_scope] and its descendents, returning entities that match [tp_predicate]
    template <typename tp_predicate, std::meta::info tp_scope = mwc_namespace>
      requires requires {std::meta::is_namespace(tp_scope) and std::is_invocable_r_v<bool, tp_predicate, const std::meta::info>;}
    consteval auto search(const tp_predicate a_predicate) {
      constexpr auto match_count = search_count<tp_predicate, tp_scope>(a_predicate);
      auto matched_entities = std::array<std::meta::info, match_count> {};
      size_t i = 0;
      const auto search_engine = [&matched_entities, &i]<typename tp_local_predicate, std::meta::info tp_local_scope>(
                                   this auto&& a_this, const tp_local_predicate a_predicate) consteval -> void {
        static constexpr auto members
          = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));

        template for (constexpr auto member : members) {
          constexpr auto recursion_candidate = verify_recursion_candidate(member, tp_local_scope);
          const bool found = a_predicate(member);

          if (found) {
            matched_entities[i] = member;
            ++i;
          }
          if constexpr (recursion_candidate.has_value()) {
            a_this.template operator()<tp_predicate, recursion_candidate.value()>(a_predicate);
          }
        }
      };

      search_engine.template operator()<tp_predicate, tp_scope>(a_predicate);
      // static_assert(match_count == 3333);
      return std::define_static_array(matched_entities);
    }

    // utility that converts a range of type reflections into a matching tuple_t
    template <auto tp>
      requires(std::meta::template_of(^^decltype(tp)) == ^^static_array_st)
    consteval auto type_info_range_tuple() {
      constexpr auto type_accumulator = []<size_t... i>([[maybe_unused]] const std::index_sequence<i...> a_index_sequence) consteval {
        static_assert((std::meta::is_type(tp.m_data[i]) and ...));

        using types = tuple_t<typename [:tp.m_data[i]:]...>;
        return types {};
      };

      return type_accumulator(std::make_index_sequence<tp.m_data.size()> {});
    }
  }
}