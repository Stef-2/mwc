#pragma once

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/instance.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/window/window.hpp"

import vulkan_hpp;
import vkfw;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class surface_ct : public handle_ct<vk::raii::SurfaceKHR> {
        public:
        using extent_t = vk::Extent2D;
        struct capabilities_st {
          using information_chain_t = vk::StructureChain<vk::PhysicalDeviceSurfaceInfo2KHR, vk::SurfacePresentModeEXT>;
          using capabilities_chain_t = vk::StructureChain<vk::SurfaceCapabilities2KHR, vk::SurfacePresentModeCompatibilityEXT,
                                                          vk::SurfacePresentScalingCapabilitiesEXT>;

          capabilities_chain_t m_capabilities_chain;
        };
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          vk::SurfaceFormat2KHR m_surface_format;
          vk::PresentModeKHR m_present_mode;
        };

        surface_ct(const window_ct& a_window, const instance_ct& a_instance, const physical_device_ct& a_physical_device,
                   const configuration_st& a_configuration = configuration_st::default_configuration());

        [[nodiscard]] auto extent() const -> const extent_t&;
        [[nodiscard]] auto capabilities() const -> const capabilities_st&;
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        extent_t m_extent;
        capabilities_st m_capabilities;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto surface_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {vk::SurfaceFormat2KHR {{vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear}},
                                 vk::PresentModeKHR::eImmediate};
      }
      template <typename tp_this>
      auto surface_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}