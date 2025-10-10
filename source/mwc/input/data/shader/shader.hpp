#pragma once

import mwc_input_resource;
import mwc_input_shader_source;
import mwc_input_spir_v;

import vulkan_hpp;

namespace mwc {
  namespace input {
    struct shader_st : public resource_st {
      using shader_stage_t = vk::ShaderStageFlagBits;

      shader_source_st m_shader_source;
      spir_v_st m_compiled_spir_v;
      vk::raii::ShaderEXT m_shader_object;
      shader_stage_t m_stage;
    }
  }
}