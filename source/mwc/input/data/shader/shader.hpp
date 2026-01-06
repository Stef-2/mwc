#pragma once

#include "mwc/graphics/vulkan/shader_object.hpp"

import mwc_input_resource;
import mwc_input_shader_source;
import mwc_input_spir_v;

import vulkan;

namespace mwc {
  namespace input {
    struct shader_st : public resource_st {
      using shader_stage_t = vk::ShaderStageFlagBits;

      shader_source_st m_shader_source;
      vector_t<spir_v_st> m_compiled_spir_v;
      optional_t<graphics::vulkan::shader_object_pipeline_ct<>> m_shader_pipeline;
      shader_stage_t m_stage;
    };
  }
}