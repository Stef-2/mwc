#include "mwc/graphics/user_interface/dear_imgui.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace mwc {
  namespace graphics {
    dear_imgui_ct::dear_imgui_ct(const window_ct& a_window, const vulkan::context_st& a_context,
                                 const vulkan::instance_ct& a_instance, const vulkan::physical_device_ct& a_physical_device,
                                 const vulkan::logical_device_ct& a_logical_device,
                                 const vulkan::queue_families_ct::family_st& a_queue_family, const vulkan::queue_ct& a_queue,
                                 const vulkan::swapchain_ct& a_swapchain, const configuration_st& a_configuration)
    : m_descriptor_pool {std::invoke([&a_logical_device, &a_configuration] -> decltype(m_descriptor_pool) {
        const auto descriptor_pool_size =
          vk::DescriptorPoolSize {vk::DescriptorType::eCombinedImageSampler, a_configuration.m_descriptor_count};

        auto expected =
          a_logical_device->createDescriptorPool(vk::DescriptorPoolCreateInfo {vk::DescriptorPoolCreateFlags {},
                                                                               /*maxSets*/ 1, descriptor_pool_size});
        contract_assert(expected);

        return std::move(expected.value());
      })},
      m_imgui_context {ImGui::CreateContext()} {
      information("initializing dear imgui");

      const auto pipeline_rendering_create_info = vk::PipelineRenderingCreateInfo {
        /*view_mask*/ 0, a_swapchain.surface().configuration().m_surface_format.surfaceFormat.format,
        a_swapchain.configuration().m_depth_stencil_attachment_configuration.m_format,
        a_swapchain.configuration().m_depth_stencil_attachment_configuration.m_format};
      const auto glfw_initialization = ImGui_ImplGlfw_InitForVulkan(*a_window.vkfw_window(), /*install_callbacks*/ true);

      contract_assert(glfw_initialization);
      auto imgui_vulkan_initialization_info =
        ImGui_ImplVulkan_InitInfo {a_context.m_vulkan_api_version.m_version,
                                   a_instance.native_handle(),
                                   a_physical_device.native_handle(),
                                   a_logical_device.native_handle(),
                                   a_queue_family.m_index,
                                   a_queue.native_handle(),
                                   *m_descriptor_pool,
                                   /*render_pass*/ nullptr,
                                   a_swapchain.image_count(),
                                   a_swapchain.image_count(),
                                   static_cast<VkSampleCountFlagBits>(a_configuration.m_sample_count),
                                   /*pipeline_cache*/ nullptr,
                                   /*subpass*/ 0,
                                   /*internal_descriptor_pool_size*/ 0,
                                   /*use_dynamic_rendering*/ true,
                                   pipeline_rendering_create_info,
                                   /*allocator*/ nullptr};

      const auto vulkan_initialization = ImGui_ImplVulkan_Init(&imgui_vulkan_initialization_info);
      contract_assert(vulkan_initialization);
    }
    dear_imgui_ct::dear_imgui_ct(dear_imgui_ct&& a_other) noexcept
    : m_descriptor_pool {std::move(a_other.m_descriptor_pool)},
      m_imgui_context {std::exchange(a_other.m_imgui_context, nullptr)} {}
    auto dear_imgui_ct::begin_frame() const -> void {
      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
    }
    auto dear_imgui_ct::render(const vk::raii::CommandBuffer& a_command_buffer) const -> void {
      auto render = false;
      ImGui::ShowDemoWindow(&render);

      ImGui::Render();
      ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *a_command_buffer);
    }
    dear_imgui_ct::~dear_imgui_ct() {
      if (m_imgui_context) {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_imgui_context);
      }
    }
  }
}
