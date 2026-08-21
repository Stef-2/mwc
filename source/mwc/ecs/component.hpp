#pragma once
//module;

#include "mwc/ecs/definition.hpp"
//export module mwc_ecs_component;

import mwc_definition;
//import mwc_ecs_definition;
import mwc_meta_search;
import mwc_metaprogramming_utility;
import mwc_concept;
import mwc_type_identity;
import mwc_observer_ptr;
import mwc_static_array;
import mwc_hash;

import std;

/*export */ namespace mwc {
  namespace ecs {
    // component type tracking

    // crtp type to be inherited by ecs component types
    template <typename tp, typename tp_underlying_pod = void>
    struct component_st : public meta::type_index_st<tp>, public meta::type_name_identity_st<tp> {
      using underlying_pod_t = tp_underlying_pod;

    };

    template <typename>
    struct component_type_list_st {
      static constexpr auto lambda = [](std::meta::info a_info) consteval -> bool {
          return std::meta::is_type(a_info)
          and std::meta::is_base_of_type(std::meta::substitute(^^component_st,
                                                    {
                                                    a_info, ^^void}),
                              a_info);
        };
      static constexpr auto component_type_infos() {
        return static_array_st {
          meta::search<decltype(lambda), ^^ecs>(lambda)
        };
      }
      static constexpr auto component_count() -> size_t {
        return meta::search<decltype(lambda), ^^ecs>(lambda).size();
      }
      static_assert(component_count() != 0);
      //static constexpr auto component_type_info_array = static_array_st {component_type_infos};

      //using component_tuple_t = decltype(meta::type_info_range_tuple<component_type_infos()>());

      /*template <size_t tp_type_index>
      using component_at_index_t = std::tuple_element_t<tp_type_index, component_tuple_t>;*/
    };

    // concept modeling component types
    template <typename tp>
    concept component_c
      = std::is_base_of_v<component_st<std::remove_cvref_t<tp>, typename std::remove_cvref_t<tp>::underlying_pod_t>,
                          std::remove_cvref_t<tp>>;

    // type erased component storage
    struct component_storage_st {
      using data_span_t = span_t<byte_t>;

      constexpr auto operator<=>(const component_storage_st& a_other) const;

      vector_t<byte_t> m_data;
      size_t m_component_size;
      archetype_component_index_t m_component_index;
    };
    // type erased view over component storage
    struct component_storage_view_st {
      constexpr auto operator<=>(const component_storage_view_st& a_other) const;

      component_storage_st::data_span_t m_data_span;
      archetype_component_index_t m_component_index;
    };
    // data used for runtime component type information
    struct component_runtime_information_st {
      constexpr auto operator<=>(const component_runtime_information_st& a_other) const;

      size_t m_component_size;
      component_index_t m_component_index;
    };

    // implementation
    constexpr auto component_storage_st::operator<=>(const component_storage_st& a_other) const {
      return m_component_index <=> a_other.m_component_index;
    }
    constexpr auto component_storage_view_st::operator<=>(const component_storage_view_st& a_other) const {
      return m_component_index <=> a_other.m_component_index;
    }
    constexpr auto component_runtime_information_st::operator<=>(const component_runtime_information_st& a_other) const {
      return m_component_index <=> a_other.m_component_index;
    }

    // sort components according to their identification in ascending order
    template <typename tp_tuple, component_c tp_x, component_c tp_y, component_c... tps>
    consteval auto component_type_sort() {
      // sort the two elements currently being operated on
      constexpr auto ascending_sort = [] {
        if constexpr (std::remove_cvref_t<tp_x>::index < std::remove_cvref_t<tp_y>::index)
          return pair_t<tp_x, tp_y> {};
        else
          return pair_t<tp_y, tp_x> {};
      };
      // determine if the elements are sorted in ascending order
      constexpr auto determine_order
        = []<bool tp_assertion = true, component_c tp_first, component_c... tp_rest>(this auto&& a_this) {
            if constexpr (not tp_assertion or sizeof...(tp_rest) == 0)
              return tp_assertion;
            else if constexpr (tp_first::index < tp_rest...[0] ::index)
              return a_this.template operator()<tp_assertion, tp_rest...>();
            else
              return a_this.template operator()<false, tp_rest...>();
          };

      if constexpr (sizeof...(tps) > 0) {
        using sorted_t = decltype(ascending_sort());
        using combined_t = decltype(std::tuple_cat(tp_tuple {}, tuple_t<decltype(sorted_t::first)> {}));
        using rest_t = decltype(std::tuple_cat(tuple_t<decltype(sorted_t::second)> {}, tuple_t<tps...> {}));
        [[maybe_unused]] auto [... rest] = rest_t {};
        return component_type_sort<combined_t, decltype(rest)...>();
      }
      // final iteration
      else {
        using result_t = decltype(std::tuple_cat(tp_tuple {}, ascending_sort()));
        [[maybe_unused]] auto [... unpack_result] = result_t {};
        // in case that this pass did not fully sort the elements, repeat
        if constexpr (not determine_order.template operator()<true, decltype(unpack_result)...>())
          return component_type_sort<tuple_t<>, decltype(unpack_result)...>();
        else
          return std::tuple_cat(tp_tuple {}, ascending_sort());
      }
    }
    template <component_c tp_component, component_c... tp_components>
    consteval auto sorted_component_types() {
      if constexpr (sizeof...(tp_components) == 0)
        return tuple_t<tp_component> {};
      else
        return component_type_sort<tuple_t<>, std::remove_cvref_t<tp_component>, std::remove_cvref_t<tp_components>...>();
    }
    template <component_c tp_component, component_c... tp_components>
    consteval auto sorted_component_reference_types() {
      [[maybe_unused]] auto [... unpack_tuple] = sorted_component_types<tp_component, tp_components...>();

      return std::declval<tuple_t<std::add_lvalue_reference_t<decltype(unpack_tuple)>...>>();
    }
    template <typename tp_operator, component_c... tp_components>
      requires(sizeof...(tp_components) > 0) and concepts::any_of_c<tp_operator, std::plus<>, std::minus<>>
    constexpr auto component_hash(component_hash_t a_initial_hash = 0) -> component_hash_t {

      static_for_loop<0, sizeof...(tp_components)>([&]<size_t tp_index> {
        // convert the component type identities to a string format, suitable for hashing
        auto identity_string = array_t<char_t, sizeof(component_hash_t)> {};
        std::to_chars(identity_string.data(), identity_string.data() + identity_string.size(),
                      std::remove_cvref_t<tp_components...[tp_index]>::index);
        if constexpr (std::is_same_v<tp_operator, std::plus<>>)
          a_initial_hash += polynomial_rolling_hash(identity_string);
        else
          a_initial_hash -= polynomial_rolling_hash(identity_string);
      });

      return a_initial_hash;
    }
    inline auto component_hash(component_hash_t a_initial_hash, const span_t<component_index_t> a_component_indices)
      -> component_hash_t {
      for (const auto component_index : a_component_indices) {
        auto identity_string = array_t<char_t, sizeof(component_hash_t)> {};
        std::to_chars(identity_string.data(), identity_string.data() + identity_string.size(), component_index);
        a_initial_hash += polynomial_rolling_hash(identity_string);
      }

      return a_initial_hash;
    }
  }
}