#pragma once

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/memory_allocator.hpp"

import mwc_definition;

import vulkan;
import vk_mem_alloc;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class image_ct : public handle_ct<vk::raii::Image> {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          vk::ImageCreateInfo m_image_create_info;
          vma::AllocationCreateInfo m_allocation_create_info;
        };
        struct layout_transition_configuration_st {
          static constexpr auto default_configuration() -> layout_transition_configuration_st;

          vk::PipelineStageFlags2 m_source_pipeline_stage;
          vk::PipelineStageFlags2 m_destination_pipeline_stage;
          vk::AccessFlags2 m_source_access_flags;
          vk::AccessFlags2 m_destination_access_flags;
          vk::ImageLayout m_old_layout;
          vk::ImageLayout m_new_layout;
          std::uint32_t m_source_queue_family;
          std::uint32_t m_destination_queue_family;
          vk::ImageSubresourceRange m_subresource_range;
        };

        using handle_ct::handle_ct;
        image_ct(const logical_device_ct& a_logical_device, const memory_allocator_ct& a_memory_allocator,
                 const configuration_st& a_configuration = configuration_st::default_configuration());

        auto record_layout_transition(const vk::raii::CommandBuffer& a_command_buffer,
                                      const layout_transition_configuration_st& a_layout_transition_configuration
                                      = layout_transition_configuration_st::default_configuration()) const -> void;

        template <typename tp_this>
        [[nodiscard]] auto allocation_info(this tp_this&& a_this) -> decltype(auto);
        template <typename tp_this>
        [[nodiscard]] auto allocation(this tp_this&& a_this) -> vma::Allocation;
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        vma::AllocationInfo m_allocation_info;
        vma::Allocation m_allocation;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto image_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {
          .m_image_create_info = {vk::ImageCreateFlags {}, vk::ImageType::e2D, vk::Format::eR8G8B8A8Srgb, vk::Extent3D {},
                                  /*mipLevels_*/ 1, /*arrayLayers_*/ 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
                                  vk::ImageUsageFlagBits::eSampled, vk::SharingMode::eExclusive},
          .m_allocation_create_info = {vma::AllocationCreateFlags {}, vma::MemoryUsage::eAutoPreferDevice,
                                       vk::MemoryPropertyFlagBits::eDeviceLocal, vk::MemoryPropertyFlagBits::eDeviceLocal}};
      }
      constexpr auto image_ct::layout_transition_configuration_st::default_configuration() -> layout_transition_configuration_st {
        return layout_transition_configuration_st {
          .m_source_pipeline_stage = vk::PipelineStageFlagBits2::eTopOfPipe,
          .m_destination_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer,
          .m_source_access_flags = {},
          .m_destination_access_flags = vk::AccessFlagBits2::eTransferWrite,
          .m_old_layout = vk::ImageLayout::eUndefined,
          .m_new_layout = vk::ImageLayout::eTransferDstOptimal,
          .m_source_queue_family = vk::QueueFamilyIgnored,
          .m_destination_queue_family = vk::QueueFamilyIgnored,
          .m_subresource_range
          = vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers}};
      }
      template <typename tp_this>
      auto image_ct::allocation_info(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_allocation_info);
      }
      template <typename tp_this>
      auto image_ct::allocation(this tp_this&& a_this) -> vma::Allocation {
        return std::forward_like<decltype(a_this)>(a_this.m_allocation);
      }
      template <typename tp_this>
      auto image_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}