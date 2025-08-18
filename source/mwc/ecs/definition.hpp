#pragma once
//module;

//export module mwc_ecs_definition;

import mwc_definition;
import mwc_type_identity;

/*export */ namespace mwc {
  namespace ecs {
    // unique global entity identifier
    using entity_index_t = uint64_t;
    // unique global component identifier
    using component_index_t = uint32_t;
    // unique global archetype identifier
    using archetype_index_t = uint16_t;
    // hash code generated from component indices
    using component_hash_t = size_t;
    // entity index within an archetype
    using archetype_entity_index_t = uint32_t;
    // component index within an archetype
    using archetype_component_index_t = uint16_t;

    static constexpr auto null_entity = entity_index_t {0};
    static constexpr auto null_component = component_index_t {0};
    static constexpr auto null_archetype = archetype_index_t {0};
  }
}