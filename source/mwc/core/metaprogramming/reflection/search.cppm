module;

export module mwc_meta_search;

import mwc_definition;
import mwc_static_array;
import mwc_namespace;

import std;

namespace mwc {
  namespace meta{
    template <std::meta::info tp_exception>
    consteval auto handle(/*const std::meta::exception& a_exception*/) -> void {
      constexpr auto exception = std::meta::extract<std::meta::exception>(tp_exception);
      typename [:exception.from():] x = 33;
      //static_assert(std::is_same_v<typename [:exception.from():], void>);
      //static_assert(false, string_view_t{exception.what()});
      //static_assert(69==420);
      //throw;
      //return exception;
    }
    consteval auto assert_recursible_scope(const std::meta::info a_outer_entity, const std::meta::info a_inner_entity) -> bool_t {
      try {
        const auto dealiased_member = a_inner_entity;//std::meta::is_type_alias(a_inner_entity) ? std::meta::dealias(a_inner_entity) : a_inner_entity;
        const auto is_complete_type = std::meta::is_complete_type(dealiased_member) and std::meta::is_class_type(dealiased_member);
        const auto is_template = std::meta::is_template(dealiased_member);
        const auto is_namespace = std::meta::is_namespace(dealiased_member);
        const auto member_of_self = std::meta::is_same_type(dealiased_member, a_outer_entity);//dealiased_member != a_outer_entity;

        if (is_namespace or (is_complete_type and not is_template and not member_of_self)) {
          return true;
        } else {
          return false;
        }
      } catch (const std::meta::exception& a_exception) {
        meta::handle<std::meta::reflect_object(a_exception)>();
        __builtin_constexpr_diag(0, "", a_exception.what());
        static_assert(0 == 2);
        // constexpr auto exception = std::meta::extract<std::meta::exception>(^^a_exception);
        //static_assert(false, string_view_t{a_exception.what()});
        //static_assert(std::meta::is_same_v<a_exception.from()>);
        constexpr auto e = meta::handle<^^a_exception>();
        static_assert(false, string_view_t {e.what(), 64});
        return false;
      }
    }
  }
}

export namespace mwc {
  namespace meta {
    // recursively search through [tp_scope] and its recursible descendents, counting the number of entities that match [tp_predicate]
    template <typename tp_predicate, std::meta::info tp_scope = mwc_namespace>
      requires requires {std::meta::is_namespace(tp_scope) and std::is_invocable_r_v<bool, tp_predicate, const std::meta::info>;}
    consteval auto search_count(const tp_predicate a_predicate) -> size_t {
        auto entity_count = size_t {0};

        const auto search_engine = [&entity_count]<typename tp_local_predicate, std::meta::info tp_local_scope>(
                                     this auto&& a_this, const tp_local_predicate a_predicate) consteval -> void {
          static constexpr auto members
            = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));

          template for (constexpr auto member : members) {
            if (a_predicate(member)) {
              ++entity_count;
            }
            if constexpr (assert_recursible_scope(member, tp_local_scope)) {
              a_this.template operator()<tp_predicate, member>(a_predicate);
            }
          }
                                     };
        search_engine.template operator()<tp_predicate, tp_scope>(a_predicate);

        return entity_count;
    }

    // recursively search through [tp_scope] and its recursible descendents, returning entities that match [tp_predicate]
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
          if (a_predicate(member)) {
            matched_entities[i] = member;
            ++i;
          }
          if constexpr (assert_recursible_scope(member, tp_local_scope)) {
            a_this.template operator()<tp_predicate, member>(a_predicate);
          }
        }
      };

      search_engine.template operator()<tp_predicate, tp_scope>(a_predicate);

      return std::define_static_array(matched_entities);
    }

    // utility that converts a range of type reflections into a matching tuple_t
    template <auto tp>
      // reflection ranges are not structural and thus can't be used as template parameters
      // they have to be wrapped in [static_array_st]
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