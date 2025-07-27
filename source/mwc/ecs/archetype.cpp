#include "mwc/ecs/archetype.hpp"

namespace mwc {
  namespace ecs {

    constexpr auto archetype_st::operator<=>(const archetype_st& a_other) const {
      return m_index <=> a_other.m_index;
    }
    archetype_st::~archetype_st() {
      for (auto& component_vector : m_component_data) {
        contract_assert(component_vector.m_data != nullptr);
        auto type_erased_vector = std::bit_cast<vector_t<byte_t>*>(component_vector.m_data);
        type_erased_vector->reserve(m_element_count * component_vector.m_data_size);
        delete type_erased_vector;
      }
    }
  }
}