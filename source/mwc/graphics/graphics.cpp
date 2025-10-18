#include "mwc/graphics/graphics.hpp"
#include "mwc/graphics/vulkan/debug.hpp"
#include "mwc/input/subsystem.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

import mwc_memory_conversion;

namespace mwc {
  namespace graphics {
    auto graphics_ct::record_mesh_data_transfer_to_device_memory(const input::scene_st& a_scene,
                                                                 const vk::raii::CommandBuffer& a_command_buffer) -> void {
      // generate a new vertex buffer
      m_vertex_buffers.emplace_back(
        m_logical_device, m_memory_allocator,
        vulkan::buffer_ct::configuration_st {
        .m_buffer_create_info
        = vk::BufferCreateInfo {vk::BufferCreateFlags {}, a_scene.m_memory_mapped_mesh_data.size(),
                                vk::BufferUsageFlagBits::eVertexBuffer bitor vk::BufferUsageFlagBits::eIndexBuffer
                                  bitor vk::BufferUsageFlagBits::eTransferDst},
        .m_allocation_create_info = vma::AllocationCreateInfo {vma::AllocationCreateFlags {}, vma::MemoryUsage::eAuto}});
      m_vertex_buffers.back().debug_name("vertex_buffer");

      // record transfer to device memory
      const auto suballocation_offset = vk::DeviceSize {pointer_cast(a_scene.m_memory_mapped_mesh_data.data())
                                                        - pointer_cast(m_common_buffer.mapped_data_pointer())};
      const auto copy_information
        = vk::BufferCopy2 {/*source_offset*/ suballocation_offset,
                           /*destination_offset*/ vk::DeviceSize {0}, a_scene.m_memory_mapped_mesh_data.size()};
      a_command_buffer.copyBuffer2(
        vk::CopyBufferInfo2 {m_common_buffer.native_handle(), m_vertex_buffers.back().native_handle(), copy_information});
    }

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
      vulkan::suballocated_memory_mapped_buffer_ct::
        configuration_st {
        .m_memory_mapped_buffer_configuration
        = vulkan::memory_mapped_buffer_ct::configuration_st {.m_buffer_create_info
                                                             = vk::BufferCreateInfo {vk::BufferCreateFlags {},
                                                                                     vk::DeviceSize {byte_count<std::mega>(256)},
                                                                                     vk::BufferUsageFlagBits::eTransferSrc,
                                                                                     vk::SharingMode::eExclusive,
                                                                                     /*pQueueFamilyIndices*/ 0},
                                                             .m_allocation_create_info
                                                             = vma::AllocationCreateInfo {vma::AllocationCreateFlags {
                                                                                          vma::AllocationCreateFlagBits::eMapped
                                                                                          bitor vma::AllocationCreateFlagBits::eHostAccessRandom},
                                                                                          vma::MemoryUsage::eAuto,
                                                                                          vk::MemoryPropertyFlags {
                                                                                          vk::MemoryPropertyFlagBits::eHostVisible},
                                                                                          vk::MemoryPropertyFlags {
                                                                                          vk::MemoryPropertyFlagBits::eDeviceLocal}}},
        .m_virtual_allocator_configuration
        = virtual_allocator_ct::configuration_st {.m_virtual_block_create_flags = vma::VirtualBlockCreateFlags {}}}},
      m_dynamic_rendering_state {m_surface},
      m_vertex_buffers {},
      m_user_interface {dear_imgui_ct {a_window, m_context, m_instance, m_physical_device, m_logical_device,
                                       m_queue_families.graphics(), m_graphics_queue, m_swapchain}},
      m_camera {camera_ct::configuration_st<camera_projection_et::e_perspective>::default_configuration()},
      m_configuration {a_configuration} {
      auto scene_cfg = input::input_subsystem_st::filesystem_st::scene_processing_configuration_st::default_configuration();
      scene_cfg.m_device_buffer = &m_common_buffer;

      input::read_scene_file("/home/billy/dev/mwc/data/mesh/Untitled.glb",
                             {{false, true, true}, {false, true, true}, &m_common_buffer});
      contract_assert(input::input_subsystem_st::filesystem_st::scene_registry.size() > 0);

      auto& frame_data = m_frame_synchronizer.m_synchronization_data[m_frame_synchronizer.m_frame_index];
      const auto& cmd = frame_data.m_command_buffer;

      cmd.begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
      record_mesh_data_transfer_to_device_memory(input::input_subsystem_st::filesystem_st::scene_registry[0], cmd);
      cmd.end();
      vk::CommandBufferSubmitInfo submit_info {*cmd};
      m_graphics_queue->submit2(vk::SubmitInfo2 {vk::SubmitFlags {}, {}, submit_info, {}});
      m_graphics_queue->waitIdle();

      auto shader_cfg = input::input_subsystem_st::filesystem_st::shader_processing_configuration_st {
        .m_device_context = {{&m_logical_device, &m_physical_device, &m_pipeline_layout}},
        true,
        true,
        true};
      mwc::file_path_t shader_path = mwc::filesystem::directory(mwc::filesystem::directory_et::e_shader);
      shader_path /= "hello_world.slang";
      mwc::input::read_shader_file(shader_path, shader_cfg);
    }

    auto graphics_ct::render() -> void {
      //imgui
      m_user_interface.begin_frame();

      //m_graphics_queue.command_pool()->reset(vk::CommandPoolResetFlagBits {});
      auto& frame_data = m_frame_synchronizer.m_synchronization_data[m_frame_synchronizer.m_frame_index];

      const auto& cmd = frame_data.m_command_buffer;
      cmd.begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

      //auto image_acquired_semaphore = frame_data.m_image_acquired_semaphore;
      //auto image_acquired_semaphore = m_logical_device->createSemaphore(vk::SemaphoreCreateInfo {}).value();
      /*auto image_acquired_fence =
        m_logical_device->createFence(vk::FenceCreateInfo {}).value();*/
      //const auto [_, image_idx] = m_swapchain.unique_handle().acquireNextImage(1000, {}, *image_acquired_fence);
      const auto [result, image_idx, /*image_acquired_semaphore,*/ render_info]
        = m_swapchain.acquire_next_image(cmd, *frame_data.m_image_acquired_semaphore);
      //m_logical_device->waitForFences(*image_acquired_fence, true, 0);
      //m_logical_device->resetFences(*image_acquired_fence);

      const auto image = m_swapchain.unique_handle().getImages().value[image_idx];
      //std::print("size: {0}\n", m_swapchain.m_image_data.size());
      auto subrsrc = vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
      cmd.clearColorImage(image, vk::ImageLayout::eColorAttachmentOptimal,
                          vk::ClearColorValue {/*(float)std::sin(std::rand())*/ 0.5f, 0.3f, 0.4f, 1.0f}, subrsrc);
      //std::cout << "frame idx: " << m_frame_synchronizer.m_frame_index << '\n';
      m_swapchain.transition_layout<vulkan::swapchain_ct::layout_state_et::e_presentation>(cmd);

      m_user_interface.generate_debug_interface();
      m_user_interface.render(cmd);
      cmd.end();

      //auto render_fence = m_logical_device->createFence(vk::FenceCreateInfo {/*vk::FenceCreateFlagBits::eSignaled*/}).value();
      auto wait_semaphores
        = vk::SemaphoreSubmitInfo {*frame_data.m_image_acquired_semaphore, 0, vk::PipelineStageFlagBits2::eTopOfPipe};
      vk::CommandBufferSubmitInfo submit_info {*cmd};
      auto signal_semaphores
        = vk::SemaphoreSubmitInfo {*frame_data.m_render_complete_semaphore, 0, vk::PipelineStageFlagBits2::eBottomOfPipe};
      m_graphics_queue->submit2(vk::SubmitInfo2 {vk::SubmitFlags {}, wait_semaphores, submit_info, signal_semaphores});

      //m_logical_device->waitForFences(*render_fence, true, 0);

      //m_logical_device->resetFences(*render_fence);
      auto present_info = vk::PresentInfoKHR {*frame_data.m_render_complete_semaphore, m_swapchain.native_handle(), image_idx};
      m_graphics_queue->presentKHR(present_info);
      //m_logical_device->waitIdle();
      if (m_frame_synchronizer.m_frame_count > 0
          and m_frame_synchronizer.m_frame_index % m_frame_synchronizer.m_frame_count == 0) {
        m_graphics_queue->waitIdle();
        m_command_pool->reset(vk::CommandPoolResetFlagBits {});
      }

      m_frame_synchronizer.m_frame_count++;
      m_frame_synchronizer.m_frame_index = (m_frame_synchronizer.m_frame_index + 1) % m_frame_synchronizer.frame_count();
    }
  }
}