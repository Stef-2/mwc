module;

export module mwc_ecs_component;

import mwc_definition;
import mwc_ecs_definition;
import mwc_type_identity;
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

    struct test0 : mwc::ecs::component_st<test0> {
      int i;
    };
    struct test1 : mwc::ecs::component_st<test1> {
      float f;
    };
    struct test2 : mwc::ecs::component_st<test2> {
      char c;
    };
    struct test3 : mwc::ecs::component_st<test3> {
      char c;
    };
  }
}