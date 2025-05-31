#pragma once

#include "mwc/core/definition/definition.hpp"

#include <limits>

//import mwc_definition;
import mwc_concept;

namespace mwc {
  namespace contract {
    constexpr bool
    validate_data_size(const concepts::contiguous_storage_c auto& a_storage) {
      return a_storage.data() != nullptr and a_storage.size() > 0 and
             a_storage.size() < std::numeric_limits<size_t>::max();
    }
  }
}