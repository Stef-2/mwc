#include "mwc/graphics/graphics.hpp"
#include "mwc/graphics/vulkan/debug.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

import mwc_memory_conversion;

namespace mwc {
  namespace graphics {
    graphics_ct::graphics_ct(const window_ct& a_window, const semantic_version_st& a_engine_version,
                             const configuration_st& a_configuration)
    : m_window {a_window},
      m_context {vulkan::context_st::configuration_st {.m_engine_version = a_engine_version}},
      m_instance {m_context},
      m_physical_device {m_instance},
      m_surface {m_window, m_instance, m_physical_device},
      m_queue_families {m_physical_device, m_surface},
      m_logical_device {m_physical_device, m_queue_families},
      m_memory_allocator {m_context, m_instance, m_physical_device, m_logical_device},
      m_swapchain {m_physical_device, m_surface, m_queue_families, m_logical_device, m_memory_allocator},
      m_pipeline_layout {m_physical_device, m_logical_device},
      m_command_pool {m_logical_device, m_queue_families},
      m_frame_synchronizer {m_logical_device, m_command_pool,
                            decltype(m_frame_synchronizer)::configuration_st {.m_frame_count = 4}},
      m_graphics_queue {m_logical_device,
                        vulkan::queue_ct::configuration_st {.m_family_index = m_queue_families.graphics().m_index}},
      m_transfer_queue {m_logical_device,
                        vulkan::queue_ct::configuration_st {.m_family_index = m_queue_families.transfer().m_index}},
      m_common_buffer {
      m_logical_device, m_memory_allocator,
      vulkan::suballocated_memory_mapped_buffer_ct::configuration_st {
      .m_memory_mapped_buffer_configuration =
        vulkan::memory_mapped_buffer_ct::configuration_st {
        .m_buffer_create_info = vk::BufferCreateInfo {vk::BufferCreateFlags {}, vk::DeviceSize {byte_count<std::mega>(256)},
                                                      vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
                                                      /*pQueueFamilyIndices*/ 0},
        .m_allocation_create_info =
          vma::AllocationCreateInfo {vma::AllocationCreateFlags {vma::AllocationCreateFlagBits::eMapped bitor
                                                                 vma::AllocationCreateFlagBits::eHostAccessRandom},
                                     vma::MemoryUsage::eAuto, vk::MemoryPropertyFlags {vk::MemoryPropertyFlagBits::eHostVisible},
                                     vk::MemoryPropertyFlags {vk::MemoryPropertyFlagBits::eDeviceLocal}}},
      .m_virtual_allocator_configuration =
        virtual_allocator_ct::configuration_st {.m_virtual_block_create_flags = vma::VirtualBlockCreateFlags {}}}},
      m_dynamic_rendering_state {m_surface},
      m_dear_imgui {a_window,         m_context,  m_instance, m_physical_device, m_logical_device, m_queue_families.graphics(),
                    m_graphics_queue, m_swapchain},
      m_configuration {a_configuration} {}

    auto graphics_ct::render() -> void {
      //imgui
      ImGui_ImplVulkan_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      //m_graphics_queue.command_pool()->reset(vk::CommandPoolResetFlagBits {});
      auto& frame_data = m_frame_synchronizer.m_synchronization_data[m_frame_synchronizer.m_frame_index];

      const auto& cmd = frame_data.m_command_buffer;
      cmd.begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

      //auto image_acquired_semaphore = frame_data.m_image_acquired_semaphore;
      //auto image_acquired_semaphore = m_logical_device->createSemaphore(vk::SemaphoreCreateInfo {}).value();
      /*auto image_acquired_fence =
        m_logical_device->createFence(vk::FenceCreateInfo {}).value();*/
      //const auto [_, image_idx] = m_swapchain.unique_handle().acquireNextImage(1000, {}, *image_acquired_fence);
      const auto [result, image_idx, /*image_acquired_semaphore,*/ render_info] =
        m_swapchain.acquire_next_image(cmd, *frame_data.m_image_acquired_semaphore);
      //m_logical_device->waitForFences(*image_acquired_fence, true, 0);
      //m_logical_device->resetFences(*image_acquired_fence);

      const auto image = m_swapchain.unique_handle().getImages()[image_idx];
      //std::print("size: {0}\n", m_swapchain.m_image_data.size());
      auto subrsrc = vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
      cmd.clearColorImage(image, vk::ImageLayout::eColorAttachmentOptimal,
                          vk::ClearColorValue {/*(float)std::sin(std::rand())*/ 0.5f, 0.3f, 0.4f, 1.0f}, subrsrc);
      //std::cout << "frame idx: " << m_frame_synchronizer.m_frame_index << '\n';
      m_swapchain.transition_layout<vulkan::swapchain_ct::layout_state_et::e_presentation>(cmd);

      bool wtf = false;
      ImGui::ShowDemoWindow(&wtf);

      ImGui::Render();
      ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *cmd);

      cmd.end();

      //auto render_fence = m_logical_device->createFence(vk::FenceCreateInfo {/*vk::FenceCreateFlagBits::eSignaled*/}).value();
      auto wait_semaphores =
        vk::SemaphoreSubmitInfo {*frame_data.m_image_acquired_semaphore, 0, vk::PipelineStageFlagBits2::eTopOfPipe};
      vk::CommandBufferSubmitInfo submit_info {*cmd};
      auto signal_semaphores =
        vk::SemaphoreSubmitInfo {*frame_data.m_render_complete_semaphore, 0, vk::PipelineStageFlagBits2::eBottomOfPipe};
      m_graphics_queue->submit2(vk::SubmitInfo2 {vk::SubmitFlags {}, wait_semaphores, submit_info, signal_semaphores});

      //m_logical_device->waitForFences(*render_fence, true, 0);

      //m_logical_device->resetFences(*render_fence);
      auto present_info = vk::PresentInfoKHR {*frame_data.m_render_complete_semaphore, m_swapchain.native_handle(), image_idx};
      m_graphics_queue->presentKHR(present_info);
      //m_logical_device->waitIdle();
      if (m_frame_synchronizer.m_frame_count > 0 and
          m_frame_synchronizer.m_frame_index % m_frame_synchronizer.m_frame_count == 0) {
        m_graphics_queue->waitIdle();
        m_command_pool->reset(vk::CommandPoolResetFlagBits {});
      }

      m_frame_synchronizer.m_frame_count++;
      m_frame_synchronizer.m_frame_index = (m_frame_synchronizer.m_frame_index + 1) % m_frame_synchronizer.frame_count();
    }
  }
}