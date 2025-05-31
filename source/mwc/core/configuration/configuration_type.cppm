module;

#include "mwc/core/definition/definition.hpp"


export module mwc_configuration_type;

export namespace mwc {
  enum class configuration_type_et : uint8_t { e_static, e_dynamic };
}