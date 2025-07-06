#include "mwc/graphics/vulkan/surface.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace {
  auto select_surface_present_mode(const mwc::graphics::vulkan::physical_device_ct& a_physical_device,
                                   const mwc::graphics::vulkan::surface_ct& a_surface,
                                   vk::PresentModeKHR a_requested_present_mode) {
    const auto available_present_modes = a_physical_device->getSurfacePresentModesKHR(a_surface.native_handle());
    if (not std::ranges::contains(available_present_modes, a_requested_present_mode)) [[unlikely]]
      return available_present_modes.front();
    else
      return a_requested_present_mode;
  }
}

namespace mwc {
  namespace graphics {
    namespace vulkan {
      surface_ct::surface_ct(const window_ct& a_window, const instance_ct& a_instance,
                             const physical_device_ct& a_physical_device, const configuration_st& a_configuration)
      : handle_ct {std::invoke([&a_window, &a_instance] -> handle_ct::handle_t {
          information(std::format("initializing vulkan surface for window titled: {0}", a_window.title()));
          const auto surface = vkfw::createWindowSurface(a_instance.native_handle(), *a_window.vkfw_window());

          return handle_ct::handle_t {a_instance.unique_handle(), surface};
        })},
        m_extent {a_window.resolution().m_width, a_window.resolution().m_height},
        m_capabilities {
        std::invoke([this, &a_physical_device, &a_configuration](
                      [[maybe_unused]] this auto&& a_template_synthesizer) -> capabilities_st::capabilities_chain_t {
          auto surface_information_chain = capabilities_st::information_chain_t {};
          // structure chain type deduction
          auto& [... capabilities_pack] = m_capabilities.m_capabilities_chain;
          surface_information_chain.get<vk::PhysicalDeviceSurfaceInfo2KHR>().surface = this->native_handle();
          surface_information_chain.get<vk::SurfacePresentModeEXT>().presentMode =
            select_surface_present_mode(a_physical_device, *this, a_configuration.m_present_mode);

          return a_physical_device->getSurfaceCapabilities2KHR<decltype(capabilities_pack)...>(
            surface_information_chain.get<vk::PhysicalDeviceSurfaceInfo2KHR>());
        })},
        m_configuration {std::invoke([this, &a_physical_device, &a_configuration] -> configuration_st {
          const auto available_formats = a_physical_device->getSurfaceFormats2KHR(this->native_handle());
          contract_assert(not available_formats.empty());
          const auto available_present_modes = a_physical_device->getSurfacePresentModesKHR(this->native_handle());
          contract_assert(not available_present_modes.empty());

          auto configuration = configuration_st {a_configuration};
          // determine if the preferred surface format is supported by the physical device
          // if not, use the first available one
          if (not std::ranges::contains(available_formats, a_configuration.m_surface_format)) {
            const auto& first_avilable_format = available_formats.front();
            warning(std::format("generated vulkan surface does not support the requested surface format:\nformat: {0}\ncolor "
                                "space: {1}\nselecting:\nformat: {2}\ncolor space: {3}",
                                vk::to_string(a_configuration.m_surface_format.surfaceFormat.format),
                                vk::to_string(a_configuration.m_surface_format.surfaceFormat.colorSpace),
                                vk::to_string(first_avilable_format.surfaceFormat.format),
                                vk::to_string(first_avilable_format.surfaceFormat.colorSpace)));
            configuration.m_surface_format = first_avilable_format;
          }

          // determine if the preferred surface present mode is supported by the physical device
          // if not, use the first available one
          const auto selected_present_mode =
            select_surface_present_mode(a_physical_device, *this, a_configuration.m_present_mode);
          if (selected_present_mode != a_configuration.m_present_mode) {
            warning(std::format("generated vulkan surface does not support the requested surface present mode:\n"
                                "{0}\nselecting:\n{1}",
                                vk::to_string(a_configuration.m_present_mode),
                                vk::to_string(selected_present_mode)));
          }
          configuration.m_present_mode = selected_present_mode;

          return configuration;
        })} {}
      auto surface_ct::extent() const -> const extent_t& {
        return m_extent;
      }
      auto surface_ct::capabilities() const -> const capabilities_st& {
        return m_capabilities;
      }
    }
  }
}
