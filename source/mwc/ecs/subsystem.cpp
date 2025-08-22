#include "mwc/ecs/subsystem.hpp"

namespace mwc {
  namespace ecs {
    auto ecs_subsystem_st::initialize() -> void {
      // generate a null archetype, with no entities nor components
      /*hash_archetype_map.emplace(component_hash_t {0},
                                 archetype_st {null_archetype, span_t<component_runtime_information_st, 0> {}});*/

      m_initialized = true;
    }
    auto ecs_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
  }
}