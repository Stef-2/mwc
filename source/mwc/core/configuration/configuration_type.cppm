module;

//#include "mwc/core/definition/definition.cppm"

export module mwc_configuration_type;

import mwc_definition;

export namespace mwc {
  enum class configuration_type_et : uint8_t { e_static, e_dynamic };
}