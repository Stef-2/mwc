module;

export module mwc_ecs_component;

import mwc_definition;
import mwc_ecs_definition;
import mwc_type_identity;
import mwc_observer_ptr;
import mwc_ctti;

import std;

export namespace mwc {
  namespace ecs {
    // component type tracking
    template <typename... tps>
    struct component_type_list_st {
      using component_tuple_t = tuple_t<tps...>;

      template <size_t tp_type_index>
      using component_at_index_t = std::tuple_element_t<tp_type_index, component_tuple_t>;
    };

    // crtp type to be inherited by ecs component types
    template <typename tp>
    struct component_st : public ctti::type_identity_st<tp, component_index_t> {
      using _ = decltype(ctti::type_list_push_back<component_type_list_st, tp>());
    };

    // concept modeling component types
    template <typename tp>
    concept component_c = std::is_base_of_v<component_st<tp>, tp>;

    // type erased component storage
    struct component_storage_st {
      void* m_data;
      size_t m_data_size;
    };

    // sort components according to their identification in ascending order
    template <typename tp_tuple, component_c tp_x, component_c tp_y, component_c... tps>
    consteval auto component_type_sort() {
      // sort the two elements currently being operated on
      constexpr auto ascending_sort = [] {
        if constexpr (tp_x::identity < tp_y::identity)
          return pair_t<tp_x, tp_y> {};
        else
          return pair_t<tp_y, tp_x> {};
      };
      // determine if the elements are sorted in ascending order
      constexpr auto determine_order = []<bool tp_assertion = true, component_c tp_first, component_c... tp_rest>(
                                         this auto&& a_this) {
        if constexpr (not tp_assertion or sizeof...(tp_rest) == 0)
          return tp_assertion;
        else if constexpr (tp_first::identity < tp_rest...[0] ::identity)
          return a_this.template operator()<tp_assertion, tp_rest...>();
        else
          return a_this.template operator()<false, tp_rest...>();
      };

      if constexpr (sizeof...(tps) > 0) {
        using sorted_t = decltype(ascending_sort());
        using combined_t = decltype(std::tuple_cat(tp_tuple {}, tuple_t<decltype(sorted_t::first)> {}));
        using rest_t = decltype(std::tuple_cat(tuple_t<decltype(sorted_t::second)> {}, tuple_t<tps...> {}));
        auto [... rest] = rest_t {};
        return component_type_sort<combined_t, decltype(rest)...>();
      }
      // final iteration
      else {
        using result_t = decltype(std::tuple_cat(tp_tuple {}, ascending_sort()));
        auto [... unpack_result] = result_t {};
        // in case that this pass did not fully sort the elements, repeat
        if constexpr (not determine_order.template operator()<true, decltype(unpack_result)...>())
          return component_type_sort<tuple_t<>, decltype(unpack_result)...>();
        else
          return std::tuple_cat(tp_tuple {}, ascending_sort());
      }
    }
    template <component_c tp_component, component_c... tp_components>
    consteval auto sorted_component_types() {
      return component_type_sort<tuple_t<>, tp_component, tp_components...>();
    }
  }
}