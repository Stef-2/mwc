module;

export module mwc_ecs_definition;

import mwc_definition;
import mwc_type_identity;

import std;

namespace mwc {}

export namespace mwc {
  namespace ecs {
    using entity_t = uint64_t;
    using component_index_t = uint8_t;

    template <typename tp>
    using component_t = type_identity_st<tp, component_index_t>;
  }
}