#pragma once

#include "mwc/graphics/vulkan/shader_object.hpp"

#include <shader-slang/slang.h>
#include <shader-slang/slang-com-ptr.h>

import mwc_input_resource;
import mwc_input_shader_source;
import mwc_input_spir_v;

namespace mwc {
  namespace input {
    struct shader_st : public resource_st {
      using shader_stage_t = vk::ShaderStageFlagBits;

      shader_source_st m_shader_source;
      Slang::ComPtr<slang::ProgramLayout> m_program_layout;
      vector_t<spir_v_st> m_compiled_spir_v;
      optional_t<graphics::vulkan::shader_object_pipeline_ct<>> m_shader_pipeline;
      shader_stage_t m_stage;
    };
  }
}