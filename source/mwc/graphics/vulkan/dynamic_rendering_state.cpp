#include "mwc/graphics/vulkan/dynamic_rendering_state.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {

      dynamic_rendering_state_ct::dynamic_rendering_state_ct(const surface_ct& a_surface, const configuration_st& a_configuration)
      : m_viewport {float32_t {0},
                    static_cast<float32_t>(a_surface.extent().height),
                    static_cast<float32_t>(a_surface.extent().width),
                    -static_cast<float32_t>(a_surface.extent().height),
                    float32_t {0},
                    float32_t {1}},
        m_scissor {vk::Offset2D {0, 0}, a_surface.extent()},
        m_configuration {a_configuration} {}
      auto dynamic_rendering_state_ct::bind(const vk::raii::CommandBuffer& a_command_buffer) const -> void {
        a_command_buffer.setViewportWithCountEXT(m_viewport);
        a_command_buffer.setScissorWithCountEXT(m_scissor);
        a_command_buffer.setCullModeEXT(m_configuration.m_cull_mode);
        a_command_buffer.setFrontFaceEXT(m_configuration.m_front_face);
        a_command_buffer.setDepthTestEnableEXT(m_configuration.m_depth_testing);
        a_command_buffer.setDepthWriteEnableEXT(m_configuration.m_depth_writing);
        a_command_buffer.setDepthCompareOpEXT(m_configuration.m_depth_compare_operation);
        a_command_buffer.setPrimitiveTopologyEXT(m_configuration.m_primitive_topology);
        a_command_buffer.setLineWidth(m_configuration.m_line_width);
        a_command_buffer.setRasterizerDiscardEnableEXT(m_configuration.m_rasterizer_discarding);
        a_command_buffer.setPolygonModeEXT(m_configuration.m_polygon_mode);
        a_command_buffer.setRasterizationSamplesEXT(m_configuration.m_sample_count);
        a_command_buffer.setSampleMaskEXT(m_configuration.m_sample_mask.first, m_configuration.m_sample_mask.second);
        a_command_buffer.setAlphaToCoverageEnableEXT(m_configuration.m_alpha_to_coverage);
        a_command_buffer.setDepthBiasEnable(m_configuration.m_depth_biasing);
        a_command_buffer.setStencilTestEnable(m_configuration.m_stencil_testing);
        a_command_buffer.setPrimitiveRestartEnable(m_configuration.m_primitive_restarting);
        a_command_buffer.setColorBlendEnableEXT(m_configuration.m_color_blend_configuration.m_first_attachment_index,
                                                m_configuration.m_color_blend_configuration.m_blending_enabled);
        a_command_buffer.setColorBlendEquationEXT(m_configuration.m_color_blend_configuration.m_first_attachment_index,
                                                  m_configuration.m_color_blend_configuration.m_blend_equation);
        a_command_buffer.setColorWriteMaskEXT(m_configuration.m_color_blend_configuration.m_first_attachment_index,
                                              m_configuration.m_color_blend_configuration.m_color_write_mask);
      }
    }
  }
}