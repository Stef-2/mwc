#pragma once

#include "mwc/ecs/component.hpp"

import mwc_math_definition;
import mwc_geometry;

namespace mwc {
  namespace ecs {
    struct null_component_st : public component_st<null_component_st> {};
    struct test0 : mwc::ecs::component_st<test0, int> {
      int i[2] = {23, 44};
    };
    struct test1 : mwc::ecs::component_st<test1, float> {
      float f = 2.0f;
    };
    struct test2 : mwc::ecs::component_st<test2, char> {
      char c = 'a';
    };
    struct test3 : mwc::ecs::component_st<test3, bool> {
      bool b = true;
    };
    struct parent_st : public component_st<parent_st, entity_index_t> {
      entity_index_t m_parent;
    };
    template <size_t tp_count = 8>
    struct static_children_st : public component_st<static_children_st<tp_count>, entity_index_t> {
      array_t<entity_index_t, tp_count> m_children;
    };
    struct position_st : public component_st<position_st, geometry::position_t<>::Scalar> {
      geometry::position_t<> m_position;
    };
    struct transformation_st : public component_st<transformation_st, geometry::transformation_t<>::Scalar> {
      geometry::transformation_t<> m_transformation;
    };
    struct camera_projection_st : public component_st<camera_projection_st, geometry::default_scalar_t> {
      math::matrix_t<geometry::default_scalar_t, 4, 4> m_projection;
    };
  }
}