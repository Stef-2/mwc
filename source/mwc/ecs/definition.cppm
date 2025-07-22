module;

export module mwc_ecs_definition;

import mwc_definition;
import mwc_type_identity;

namespace mwc {}

export namespace mwc {
  namespace ecs {
    using entity_t = uint64_t;
    using component_index_t = uint16_t;
    using archetype_hash_t = size_t;
  }
}