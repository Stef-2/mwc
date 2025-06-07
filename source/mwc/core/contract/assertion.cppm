module;
//#pragma once

//#include "mwc/core/definition/definition.cppm"

//#include <limits>
export module mwc_contract_assertion;

import mwc_definition;
import mwc_concept;

import std;

export namespace mwc {
  namespace contract {
    constexpr bool
    validate_storage(const concepts::contiguous_storage_c auto& a_storage) {
      return a_storage.data() != nullptr and a_storage.size() > 0 and
             a_storage.size() < std::numeric_limits<size_t>::max();
    }
  }
}