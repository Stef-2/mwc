module;

export module mwc_meta_search;

import mwc_definition;
import mwc_static_array;
import mwc_static_string;
import mwc_namespace;

import std;

namespace mwc {
  namespace mwc {}
}
namespace mwc {
  namespace meta {
    enum class parse_mode_et : uint8_t {
      e_count,
      e_search
    };
    consteval auto assert_recursible_scope(const std::meta::info a_outer_entity, const std::meta::info a_inner_entity) -> bool_t {
      using namespace std::meta;

      const auto dinner = dealias(a_inner_entity);
      const auto douter = dealias(a_outer_entity);

      const auto inner_complete_type = is_complete_type(dinner) and is_class_type(dinner);
      const auto outer_complete_type = is_complete_type(douter) and is_class_type(douter);
      const auto inner_namespace = is_namespace(dinner) and a_inner_entity != ^^std;
      const auto outer_namespace = is_namespace(douter);

      const auto template_type = is_template(dinner);
      const auto member_of_self = is_complete_type(dinner) and is_complete_type(douter) and is_same_type(dinner, douter);
      const auto self = dinner == douter;

      if (inner_namespace or inner_complete_type) {
        //if (is_namespace or is_complete_type) {
        return true;
      } else {
        return false;
      }
    }
  }
}

export namespace mwc {
  namespace meta {
    // recursively search through [tp_scope] and its recursible descendents, counting the number of entities that match [tp_predicate]
    template <typename tp_predicate, std::meta::info tp_scope = mwc_namespace>
      requires requires {
                 std::meta::is_namespace(tp_scope) and std::is_invocable_r_v<bool, tp_predicate, const std::meta::info>;
               }
    consteval auto search_count(const tp_predicate a_predicate) -> size_t {
      auto parsed_entities = vector_t<std::meta::info> {};
      auto entity_count = size_t {0};

      const auto search_engine = [&entity_count, &parsed_entities]<typename tp_local_predicate, std::meta::info tp_local_scope>(
                                   this auto&& a_this,
                                   const tp_local_predicate a_predicate) consteval -> void {
        static constexpr auto members
          = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));
        parsed_entities.push_back(tp_local_scope);

        template for (constexpr auto member : members) {
          /*if constexpr (member == ^^::mwc::meta) {
            static_assert(false);
          }*/
          const auto already_parsed = std::ranges::contains(parsed_entities, tp_local_scope);
          const auto match = a_predicate(member);
          if (match) {
            ++entity_count;
          }

          constexpr auto recursible_scope = assert_recursible_scope(tp_local_scope, member);
          if (recursible_scope and not already_parsed and not match) {
            a_this.template operator()<tp_predicate, member>(a_predicate);
          }
        }
      };
      search_engine.template operator()<tp_predicate, tp_scope>(a_predicate);

      return entity_count;
    }

    // recursively search through [tp_scope] and its recursible descendents, returning entities that match [tp_predicate]
    // template <typename tp_predicate, std::meta::info tp_scope = mwc_namespace>
    //   requires requires {
    //              std::meta::is_namespace(tp_scope) and std::is_invocable_r_v<bool, tp_predicate, const std::meta::info>;
    //            }
    // consteval auto search(const tp_predicate a_predicate) {
    //   auto parsed_entities = vector_t<std::meta::info> {};
    //   constexpr auto match_count = search_count<tp_predicate, tp_scope>(a_predicate);
    //   auto matched_entities = array_t<std::meta::info, match_count> {};
    //   size_t i = 0;
    //   const auto search_engine
    //     = [&matched_entities, &parsed_entities, &i]<typename tp_local_predicate, std::meta::info tp_local_scope>(
    //         this auto&& a_this,
    //         const tp_local_predicate a_predicate) consteval -> void {
    //     static constexpr auto members
    //       = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));
    //
    //     parsed_entities.push_back(tp_local_scope);
    //     template for (constexpr auto member : members) {
    //       constexpr auto recursible_scope = assert_recursible_scope(tp_local_scope, member);
    //       const auto already_parsed = std::ranges::contains(parsed_entities, tp_local_scope);
    //       const auto match = a_predicate(member);
    //       if (match) {
    //         /*
    //         if (std::ranges::contains(matched_entities, member)) {
    //           static_assert(std::is_same_v<typename[:member:], char****>);
    //         }*/
    //         matched_entities[i] = member;
    //         ++i;
    //       }
    //
    //       if (recursible_scope and not already_parsed and not match) {
    //         a_this.template operator()<tp_predicate, member>(a_predicate);
    //       }
    //     }
    //   };
    //
    //   search_engine.template operator()<tp_predicate, tp_scope>(a_predicate);
    //
    //   return std::define_static_array(matched_entities);
    // }

    template <typename tp_predicate, std::meta::info tp_scope = mwc_namespace>
      requires requires {
                 std::meta::is_namespace(tp_scope) and std::is_invocable_r_v<bool, tp_predicate, const std::meta::info>;
               }
    consteval auto search(const tp_predicate a_predicate) {
      auto parsed_entities = vector_t<std::meta::info> {};
      auto matched_entities = vector_t<std::meta::info> {};
      auto i = size_t {0};

      constexpr auto search_engine = []<typename tp_local_predicate, std::meta::info tp_local_scope, parse_mode_et tp_parse_mode>(
                                       this auto&& a_this, const tp_local_predicate a_predicate,
                                       vector_t<std::meta::info> a_matched_entities = {}) consteval {
        static constexpr auto members
          = std::define_static_array(std::meta::members_of(tp_local_scope, std::meta::access_context::current()));
        /*if constexpr (tp_parse_mode == parse_mode_et::e_search) {
          parsed_entities.push_back(tp_local_scope);
        }*/

        template for (constexpr auto member : members) {
          //const auto already_parsed = std::ranges::contains(parsed_entities, tp_local_scope);
          const auto match = a_predicate(member);
          if (match) {
            a_matched_entities.push_back(member);
          }

          constexpr auto recursible_scope = assert_recursible_scope(tp_local_scope, member);
          if (recursible_scope /*not already_parsed and*/) {
            return a_this.template operator()<tp_predicate, member, tp_parse_mode>(a_predicate, std::move(a_matched_entities));
          }
        }

        if constexpr (tp_parse_mode == parse_mode_et::e_search)
          return a_matched_entities;
        else
          return a_matched_entities.size();
      };
      constexpr auto cnt = search_engine.template operator()<tp_predicate, tp_scope, parse_mode_et::e_search>(a_predicate);
    }

    // utility that converts a range of type reflections into a matching tuple_t
    template <auto tp>
    // reflection ranges are not structural and thus can't be used as template parameters
    // they have to be wrapped in [static_array_st]
      requires(std::meta::template_of(^^decltype(tp)) == ^^static_array_st)
    consteval auto type_info_range_tuple() {
      constexpr auto type_accumulator
        = []<size_t... i>([[maybe_unused]] const std::index_sequence<i...> a_index_sequence) consteval {
            static_assert((std::meta::is_type(tp.m_data[i]) and ...));

            using types = tuple_t<typename[:tp.m_data[i]:]...>;
            return types {};
          };

      return type_accumulator(std::make_index_sequence<tp.m_data.size()> {});
    }
  }
}
