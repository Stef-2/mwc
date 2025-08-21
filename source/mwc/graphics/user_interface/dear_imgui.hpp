#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/window/window.hpp"
#include "mwc/graphics/vulkan/context.hpp"
#include "mwc/graphics/vulkan/instance.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/queue_families.hpp"
#include "mwc/graphics/vulkan/queue.hpp"
#include "mwc/graphics/vulkan/swapchain.hpp"

#include "imgui.h"

import mwc_definition;
import mwc_type_mobility;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    class dear_imgui_ct : public irreproducible_st {
      public:
      struct configuration_st {
        static constexpr auto default_configuration() -> configuration_st;

        vk::SampleCountFlagBits m_sample_count;
        uint32_t m_descriptor_count;
      };

      static auto centered_text(const string_view_t a_string) -> void;

      dear_imgui_ct(const window_ct& a_window,
                    const vulkan::context_st& a_context,
                    const vulkan::instance_ct& a_instance,
                    const vulkan::physical_device_ct& a_physical_device,
                    const vulkan::logical_device_ct& a_logical_device,
                    const vulkan::queue_families_ct::family_st& a_queue_family,
                    const vulkan::queue_ct& a_queue,
                    const vulkan::swapchain_ct& a_swapchain,
                    const configuration_st& a_configuration = configuration_st::default_configuration());

      dear_imgui_ct(dear_imgui_ct&&) noexcept;
      auto operator=(dear_imgui_ct&&) noexcept -> dear_imgui_ct& = delete;

      auto begin_frame() const -> void;
      auto render(const vk::raii::CommandBuffer& a_command_buffer) const -> void;

      ~dear_imgui_ct();

      private:
      vk::raii::DescriptorPool m_descriptor_pool;
      ImGuiContext* m_imgui_context;
    };

    // implementation
    constexpr auto dear_imgui_ct::configuration_st::default_configuration() -> configuration_st {
      return configuration_st {.m_sample_count = vk::SampleCountFlagBits::e1, .m_descriptor_count = 32};
    }

  }
}