#include "mwc/ecs/entity.hpp"

namespace mwc {
  namespace ecs {
    constexpr entity_ct::entity_ct(entity_ct&& a_other) noexcept
    : m_index {std::exchange(a_other.m_index, null_entity_index)} {}
    constexpr auto entity_ct::operator=(entity_ct&& a_other) noexcept -> entity_ct& {
      m_index = std::exchange(a_other.m_index, null_entity_index);

      return *this;
    }
    constexpr entity_ct::~entity_ct() {
      if (m_index != null_entity_index)
        destroy_entity(m_index);
    }
    constexpr auto entity_ct::operator<=>(const entity_ct& a_other) const {
      return m_index <=> a_other.m_index;
    }
    constexpr entity_ct::operator unsigned long int() const {
      return m_index;
    }
    constexpr auto entity_ct::index() const -> entity_index_t {
      return m_index;
    }
  }
}