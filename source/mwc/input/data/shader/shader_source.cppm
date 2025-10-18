module;

export module mwc_input_shader_source;

import mwc_definition;

export namespace mwc {
  namespace input {
    struct shader_source_st {
      using storage_t = string_t;

      storage_t m_source_code;
    };
  }
}