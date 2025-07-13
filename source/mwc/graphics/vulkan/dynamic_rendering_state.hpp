#pragma once

#include "mwc/graphics/vulkan/surface.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class dynamic_rendering_state_ct {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          struct color_blend_configuration_st {
            static constexpr auto attachment_count = 1;
            static constexpr auto default_configuration() -> color_blend_configuration_st;

            uint32_t m_first_attachment_index;
            array_t<vk::Bool32, attachment_count> m_blending_enabled;
            array_t<vk::ColorBlendEquationEXT, attachment_count> m_blend_equation;
            array_t<vk::ColorComponentFlags, attachment_count> m_color_write_mask;
          };

          vk::CullModeFlags m_cull_mode = vk::CullModeFlagBits::eBack;
          vk::FrontFace m_front_face = vk::FrontFace::eClockwise;
          bool_t m_depth_testing = true;
          bool_t m_depth_writing = true;
          vk::CompareOp m_depth_compare_operation = vk::CompareOp::eLessOrEqual;
          vk::PrimitiveTopology m_primitive_topology = vk::PrimitiveTopology::eTriangleList;
          bool_t m_rasterizer_discarding = false;
          vk::PolygonMode m_polygon_mode = vk::PolygonMode::eFill;
          float32_t m_line_width = float32_t {1.0};
          vk::SampleCountFlagBits m_sample_count = vk::SampleCountFlagBits::e1;
          pair_t<vk::SampleCountFlagBits, array_t<uint32_t, 1>> m_sample_mask = {vk::SampleCountFlagBits::e1, {1}};
          bool_t m_alpha_to_coverage = false;
          bool_t m_depth_biasing = false;
          bool_t m_stencil_testing = false;
          bool_t m_primitive_restarting = false;
          color_blend_configuration_st m_color_blend_configuration;
        };

        dynamic_rendering_state_ct(const surface_ct& a_surface,
                                   const configuration_st& a_configuration = configuration_st::default_configuration());

        auto bind(const vk::raii::CommandBuffer& a_command_buffer) const -> void;
        [[nodiscard]] auto viewport() const -> vk::Viewport;
        [[nodiscard]] auto scissor() const -> vk::Rect2D;
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        vk::Viewport m_viewport;
        vk::Rect2D m_scissor;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto dynamic_rendering_state_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {.m_cull_mode = vk::CullModeFlagBits::eBack,
                                 .m_front_face = vk::FrontFace::eClockwise,
                                 .m_depth_testing = true,
                                 .m_depth_writing = true,
                                 .m_depth_compare_operation = vk::CompareOp::eLessOrEqual,
                                 .m_primitive_topology = vk::PrimitiveTopology::eTriangleList,
                                 .m_rasterizer_discarding = false,
                                 .m_polygon_mode = vk::PolygonMode::eFill,
                                 .m_line_width = float32_t {1.0},
                                 .m_sample_count = vk::SampleCountFlagBits::e1,
                                 .m_sample_mask =
                                   pair_t<vk::SampleCountFlagBits, array_t<uint32_t, 1>> {vk::SampleCountFlagBits::e1, {1}},
                                 .m_alpha_to_coverage = false,
                                 .m_depth_biasing = false,
                                 .m_stencil_testing = false,
                                 .m_primitive_restarting = false,
                                 .m_color_blend_configuration = color_blend_configuration_st::default_configuration()};
      }
      constexpr auto dynamic_rendering_state_ct::configuration_st::color_blend_configuration_st::default_configuration()
        -> color_blend_configuration_st {
        return color_blend_configuration_st {
          .m_first_attachment_index = 0,
          .m_blending_enabled = array_t<vk::Bool32, attachment_count> {vk::True},
          .m_blend_equation = array_t<vk::ColorBlendEquationEXT, attachment_count> {vk::ColorBlendEquationEXT {
          vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd, vk::BlendFactor::eOne,
          vk::BlendFactor::eZero, vk::BlendOp::eAdd}},
          .m_color_write_mask = array_t<vk::ColorComponentFlags, attachment_count> {
          vk::ColorComponentFlags {vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                   vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA}}};
      }
      template <typename tp_this>
      auto dynamic_rendering_state_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}