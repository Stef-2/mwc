module;

export module mwc_input_shader_source;

import mwc_definition;
import mwc_input_resource;

import vulkan_hpp;

export namespace mwc {
  namespace input {
    struct shader_source_st : public resource_st {
      using shader_stage_t = vk::ShaderStageFlagBits;

      string_t m_source_code;
      shader_stage_t m_stage;
    };
  }
}