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
    auto archetype_st::component_index(const component_index_t a_component_index) const -> archetype_component_index_t {
      for (const auto& component : m_component_data)
        if (component.m_component_index == a_component_index)
          return component.m_component_index;

      contract_assert(false);
      std::unreachable();
    }
  }
}