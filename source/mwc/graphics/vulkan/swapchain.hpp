#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/surface.hpp"
#include "mwc/graphics/vulkan/queue_families.hpp"
#include "mwc/graphics/vulkan/memory_allocator.hpp"
#include "mwc/graphics/vulkan/image.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class swapchain_ct : public handle_ct<vk::raii::SwapchainKHR> {
        public:
        using image_index_t = decltype(std::declval<handle_t>().acquireNextImage({}).second);
        using image_timeout_t = uint64_t;

        enum class layout_state_et { e_rendering, e_presentation };

        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          struct attachment_operations_configuration_st {
            vk::AttachmentLoadOp m_load_operation;
            vk::AttachmentStoreOp m_store_operation;
          };

          struct color_attachment_configuration_st {
            attachment_operations_configuration_st m_attachment_operations;
            vk::ImageLayout m_layout;
            vk::ClearColorValue m_clear_color;
          };

          struct depth_stencil_attachment_configuration_st {
            attachment_operations_configuration_st m_attachment_operations;
            vk::Format m_format;
            vk::ImageLayout m_layout;
            vk::ClearDepthStencilValue m_clear_value;
          };

          color_attachment_configuration_st m_color_attachment_configuration;
          depth_stencil_attachment_configuration_st m_depth_stencil_attachment_configuration;

          image_index_t m_image_count;
          vk::SharingMode m_sharing_mode;
          vk::SurfaceTransformFlagBitsKHR m_transform;
          vk::CompositeAlphaFlagBitsKHR m_alpha;

          image_timeout_t m_image_acquisition_timeout;
        };

        struct acquired_image_data_st {
          vk::Result m_result;
          image_index_t m_image_index;
          vk::RenderingInfo m_rendering_information;
        };

        swapchain_ct(const physical_device_ct& a_physical_device,
                     const surface_ct& a_surface,
                     const queue_families_ct& a_queue_families,
                     const logical_device_ct& a_logical_device,
                     const memory_allocator_ct& a_memory_allocator,
                     const configuration_st& a_configuration = configuration_st::default_configuration());

        template <layout_state_et tp_state>
        auto transition_layout(const vk::raii::CommandBuffer& a_command_buffer) const -> void;
        auto acquire_next_image(const vk::raii::CommandBuffer& a_command_buffer, const vk::Semaphore& a_semaphore_to_signal,
                                const optional_t<const vk::Fence> a_fence_to_signal = std::nullopt) -> acquired_image_data_st;

        auto surface() const -> const surface_ct&;
        auto image_views() const -> const span_t<const vk::raii::ImageView>;
        auto current_image_index() const -> image_index_t;
        auto depth_stencil_buffer() const -> const image_ct&;
        auto depth_stencil_view() const -> const vk::raii::ImageView&;
        auto image_count() const -> image_index_t;
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        //private:
        const surface_ct& m_surface;

        vector_t<vk::raii::ImageView> m_image_views;
        image_index_t m_current_image_index;

        // depth-stencil buffer
        image_ct m_depth_stencil_buffer;
        vk::raii::ImageView m_depth_stencil_view;

        // dynamic rendering attachments
        vk::RenderingAttachmentInfo m_color_attachment;
        vk::RenderingAttachmentInfo m_depth_stencil_attachment;

        configuration_st m_configuration;
      };

      // implementation
      constexpr auto swapchain_ct::configuration_st::default_configuration() -> configuration_st {

        return configuration_st {
          .m_color_attachment_configuration = {.m_attachment_operations = {.m_load_operation = vk::AttachmentLoadOp::eClear,
                                                                           .m_store_operation = vk::AttachmentStoreOp::eStore},
                                               .m_layout = vk::ImageLayout::eAttachmentOptimal,
                                               .m_clear_color = vk::ClearColorValue {float32_t {1.0}, 1.0, 1.0, 1.0}},
          .m_depth_stencil_attachment_configuration = {.m_attachment_operations = {.m_load_operation = vk::AttachmentLoadOp::eClear,
                                                                                   .m_store_operation =
                                                                                     vk::AttachmentStoreOp::eDontCare},
                                                       .m_format = vk::Format::eD24UnormS8Uint,
                                                       .m_layout = vk::ImageLayout::eAttachmentOptimal,
                                                       .m_clear_value = vk::ClearDepthStencilValue {1.0, 1u}},
          .m_image_count = 2,
          .m_sharing_mode = vk::SharingMode::eExclusive,
          .m_transform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
          .m_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
          .m_image_acquisition_timeout = std::numeric_limits<image_timeout_t>::max()};
      }
      template <swapchain_ct::layout_state_et tp_state>
      auto swapchain_ct::transition_layout(const vk::raii::CommandBuffer& a_command_buffer) const -> void {
        constexpr auto queue_ownership = uint32_t {0};

        constexpr auto undefined_layout = vk::ImageLayout::eUndefined;
        constexpr auto present_layout = vk::ImageLayout::ePresentSrcKHR;
        constexpr auto color_layout = vk::ImageLayout::eColorAttachmentOptimal;
        constexpr auto depth_stencil_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        constexpr auto pipeline_stage = (tp_state == layout_state_et::e_rendering) ? vk::PipelineStageFlagBits2::eTopOfPipe
                                                                                   : vk::PipelineStageFlagBits2::eBottomOfPipe;

        constexpr auto color_transition_to_rendering =
          (tp_state == layout_state_et::e_rendering) ? undefined_layout : color_layout;
        constexpr auto depth_stencil_transition_to_rendering =
          (tp_state == layout_state_et::e_rendering) ? undefined_layout : depth_stencil_layout;

        constexpr auto color_transition_to_presentation =
          (tp_state == layout_state_et::e_presentation) ? present_layout : color_layout;
        constexpr auto depth_stencil_transition_to_presentation =
          (tp_state == layout_state_et::e_presentation) ? present_layout : depth_stencil_layout;

        const auto barriers = array_t<vk::ImageMemoryBarrier2, 2> {
          vk::ImageMemoryBarrier2 {pipeline_stage, vk::AccessFlags2 {}, vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                                   vk::AccessFlags2 {}, color_transition_to_rendering, color_transition_to_presentation,
                                   queue_ownership, queue_ownership, this->unique_handle().getImages()[m_current_image_index],
                                   vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}},
          vk::ImageMemoryBarrier2 {
          pipeline_stage, vk::AccessFlags2 {},
          vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests, vk::AccessFlags2 {},
          depth_stencil_transition_to_rendering, depth_stencil_transition_to_presentation, queue_ownership, queue_ownership,
          **m_depth_stencil_buffer,
          vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eDepth bitor vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1}}};

        const auto dependency_info =
          vk::DependencyInfo {vk::DependencyFlags {}, /*vk::MemoryBarrier2*/ {}, /*vk::BufferMemoryBarrier2*/ {}, barriers};

        a_command_buffer.pipelineBarrier2(dependency_info);
      }
      template <typename tp_this>
      auto swapchain_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}
