#pragma once

#include "mwc/core/contract/definition.hpp"

import mwc_definition;

import std;

namespace mwc {
  template <typename tp>
    requires std::is_pointer_v<tp>
  constexpr auto pointer_cast(const tp a_pointer) -> uintptr_t pre(a_pointer != nullptr) {
    return std::bit_cast<uintptr_t>(a_pointer);
  }
}
