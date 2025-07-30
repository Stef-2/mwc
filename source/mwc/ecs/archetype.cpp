#include "mwc/ecs/archetype.hpp"

namespace mwc {
  namespace ecs {

    constexpr auto archetype_st::operator<=>(const archetype_st& a_other) const {
      return m_index <=> a_other.m_index;
    }
    auto archetype_st::component_count() const -> archetype_component_index_t {
      return m_component_data.size();
    }
    auto archetype_st::entity_count() const -> archetype_entity_index_t {
      return m_entity_count;
    }
  }
}