#pragma once

#include "mwc/ecs/component.hpp"

import mwc_geometry;

namespace mwc {
  namespace ecs {
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
    struct position_st : public component_st<position_st /*, geometry::position_t<>::Scalar*/> {
      geometry::position_t<> m_position;
    };
  }
}