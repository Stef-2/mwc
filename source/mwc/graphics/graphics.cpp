#include "mwc/graphics/graphics.hpp"

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
      m_frame_synchornizer {m_logical_device, decltype(m_frame_synchornizer)::configuration_st {.m_frame_count = 2}},
      m_graphics_queue {m_logical_device, m_queue_families},
      m_configuration {a_configuration} {}

    auto graphics_ct::render() -> void {
      m_graphics_queue.command_pool()->reset(vk::CommandPoolResetFlagBits {});
      const auto& cmd = m_graphics_queue.command_buffers().front();
      cmd.begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

      auto image_acquired_fence =
        m_logical_device->createFence(vk::FenceCreateInfo {/*vk::FenceCreateFlagBits::eSignaled*/}).value();
      const auto [_, image_idx] = m_swapchain.unique_handle().acquireNextImage(1000, {}, *image_acquired_fence);

      m_logical_device->waitForFences(*image_acquired_fence, true, 0);

      m_logical_device->resetFences(*image_acquired_fence);

      const auto image = m_swapchain.unique_handle().getImages()[image_idx];
      auto subrsrc = vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
      cmd.clearColorImage(image, vk::ImageLayout::eGeneral,
                          vk::ClearColorValue {(float)std::sin(std::rand()), 0.0f, 0.0f, 1.0f}, subrsrc);

      cmd.end();

      auto render_fence = m_logical_device->createFence(vk::FenceCreateInfo {/*vk::FenceCreateFlagBits::eSignaled*/}).value();
      vk::CommandBufferSubmitInfo submit_info {*cmd};
      m_graphics_queue->submit2(vk::SubmitInfo2 {{}, {}, submit_info}, *render_fence);

      m_logical_device->waitForFences(*render_fence, true, 0);

      m_logical_device->resetFences(*render_fence);
      auto present_info = vk::PresentInfoKHR {{}, m_swapchain.native_handle(), image_idx};
      m_graphics_queue->presentKHR(present_info);
      m_logical_device->waitIdle();
    }
  }
}