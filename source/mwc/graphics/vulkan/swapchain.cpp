#include "mwc/graphics/vulkan/swapchain.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace {
  auto clamp_image_count(const mwc::graphics::vulkan::swapchain_ct::image_index_t a_image_count,
                         const mwc::graphics::vulkan::surface_ct& a_surface) {
    const auto& surface_capabilities =
      a_surface.capabilities().m_capabilities_chain.get<vk::SurfaceCapabilities2KHR>().surfaceCapabilities;

    return std::clamp(a_image_count, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);
  }
}

namespace mwc {
  namespace graphics {
    namespace vulkan {
      swapchain_ct::swapchain_ct(const physical_device_ct& a_physical_device, const surface_ct& a_surface,
                                 const queue_families_ct& a_queue_families, const logical_device_ct& a_logical_device,
                                 const memory_allocator_ct& a_memory_allocator, const configuration_st& a_configuration)
      : handle_ct {std::invoke([&a_surface, &a_queue_families, &a_logical_device, a_configuration] -> handle_t {
          // determine if a single queue family supports both graphics and present operations
          auto combined_queue_family_indices = array_t<queue_families_ct::family_st::index_t, 2> {
            a_queue_families.graphics().m_index, a_queue_families.present().m_index};
          const auto combined_graphics_and_present = a_queue_families.supports_combined_graphics_and_present_family();
          const auto queue_family_indices = combined_graphics_and_present ? span_t {combined_queue_family_indices.begin(), 1}
                                                                          : span_t {combined_queue_family_indices.begin(), 2};

          const auto image_count = clamp_image_count(a_configuration.m_image_count, a_surface);
          const auto& surface_configuration = a_surface.configuration();
          information(std::format("initializing vulkan swapchain:\nimage count: {0}\nimage format: {1}\ncolor space: "
                                  "{2}\nimage extent: {3}x{4}\npresent mode: {5}",
                                  image_count, vk::to_string(surface_configuration.m_surface_format.surfaceFormat.format),
                                  vk::to_string(surface_configuration.m_surface_format.surfaceFormat.colorSpace),
                                  a_surface.extent().width, a_surface.extent().height,
                                  vk::to_string(surface_configuration.m_present_mode)));

          const auto swapchain_create_info =
            vk::SwapchainCreateInfoKHR {vk::SwapchainCreateFlagsKHR {},
                                        a_surface.native_handle(),
                                        image_count,
                                        surface_configuration.m_surface_format.surfaceFormat.format,
                                        surface_configuration.m_surface_format.surfaceFormat.colorSpace,
                                        a_surface.extent(),
                                        /*imageArrayLayers_*/ 1,
                                        vk::ImageUsageFlags {vk::ImageUsageFlagBits::eColorAttachment},
                                        a_configuration.m_sharing_mode,
                                        queue_family_indices,
                                        a_configuration.m_transform,
                                        a_configuration.m_alpha,
                                        surface_configuration.m_present_mode};

          auto expected = a_logical_device->createSwapchainKHR(swapchain_create_info);
          contract_assert(expected);

          return std::move(expected.value());
        })},
        m_surface {a_surface},
        m_image_data {std::invoke([this, &a_surface, &a_logical_device] -> decltype(m_image_data) {
          const auto images = this->unique_handle().getImages();

          auto result = decltype(m_image_data) {};
          result.reserve(images.size());
          m_image_data.reserve(images.size());

          // for each image in the swapchain, create image views and frame synchronization data
          for (const auto& image : images) {
            auto fence_expected = a_logical_device->createFence(vk::FenceCreateInfo {vk::FenceCreateFlagBits::eSignaled});
            auto image_acquired_semaphore_expected = a_logical_device->createSemaphore(vk::SemaphoreCreateInfo {});
            auto render_finished_semaphore_expected = a_logical_device->createSemaphore(vk::SemaphoreCreateInfo {});
            auto image_view_expected = a_logical_device->createImageView(
              vk::ImageViewCreateInfo {vk::ImageViewCreateFlags {}, image, vk::ImageViewType::e2D,
                                       a_surface.configuration().m_surface_format.surfaceFormat.format, vk::ComponentMapping {},
                                       vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, 0, vk::RemainingMipLevels, 0,
                                                                  vk::RemainingArrayLayers}});
            contract_assert(fence_expected and image_acquired_semaphore_expected and render_finished_semaphore_expected and
                            image_view_expected);

            result.emplace_back(internal_frame_synchronization_data_st {std::move(fence_expected.value()),
                                                                        std::move(image_acquired_semaphore_expected.value()),
                                                                        std::move(render_finished_semaphore_expected.value())},
                                std::move(image_view_expected.value()));
          }

          return result;
        })},
        m_depth_stencil_buffer {
        a_logical_device, a_memory_allocator,
        image_ct::configuration_st {
        vk::ImageCreateInfo {
        vk::ImageCreateFlags {}, vk::ImageType::e2D, a_configuration.m_depth_stencil_attachment_configuration.m_format,
        vk::Extent3D {a_surface.extent(), 1}, /*mipLevels_*/ 1, /*arrayLayers_*/ 1, vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, a_configuration.m_sharing_mode},
        vma::AllocationCreateInfo {vma::AllocationCreateFlags {}, vma::MemoryUsage::eAutoPreferDevice,
                                   vk::MemoryPropertyFlagBits::eDeviceLocal, vk::MemoryPropertyFlagBits::eDeviceLocal}}},
        m_depth_stencil_view {a_logical_device
                                ->createImageView(vk::ImageViewCreateInfo {
                                vk::ImageViewCreateFlags {}, m_depth_stencil_buffer.native_handle(), vk::ImageViewType::e2D,
                                a_configuration.m_depth_stencil_attachment_configuration.m_format, vk::ComponentMapping {},
                                vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eDepth bitor vk::ImageAspectFlagBits::eStencil,
                                                           0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers}})
                                .value()},
        m_color_attachment {vk::ImageView {/*imageView_*/},
                            a_configuration.m_color_attachment_configuration.m_layout,
                            vk::ResolveModeFlagBits {},
                            vk::ImageView {/*resolveImageView_*/},
                            vk::ImageLayout {/*a_resolveImageLayout_configuration*/},
                            a_configuration.m_color_attachment_configuration.m_attachment_operations.m_load_operation,
                            a_configuration.m_color_attachment_configuration.m_attachment_operations.m_store_operation,
                            a_configuration.m_color_attachment_configuration.m_clear_color},
        m_depth_stencil_attachment {
        vk::ImageView {/*imageView_*/},
        a_configuration.m_depth_stencil_attachment_configuration.m_layout,
        vk::ResolveModeFlagBits {},
        vk::ImageView {/*resolveImageView_*/},
        vk::ImageLayout {/*a_resolveImageLayout_configuration*/},
        a_configuration.m_depth_stencil_attachment_configuration.m_attachment_operations.m_load_operation,
        a_configuration.m_depth_stencil_attachment_configuration.m_attachment_operations.m_store_operation,
        a_configuration.m_depth_stencil_attachment_configuration.m_clear_value} {}
    }
  }
}