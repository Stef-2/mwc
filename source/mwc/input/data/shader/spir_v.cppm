module;

export module mwc_input_spir_v;

import mwc_definition;
import mwc_input_shader_source;

export namespace mwc {
  namespace input {
    struct spir_v_st {
      using spir_v_bytecode_t = uint32_t;
      using storage_t = vector_t<spir_v_bytecode_t>;

      storage_t m_data;
    };
  }
}