#include "mwc/ecs/archetype.hpp"

namespace mwc {
  namespace ecs {

    constexpr auto archetype_st::operator<=>(const archetype_st& a_other) const {
      return m_index <=> a_other.m_index;
    }
  }
}