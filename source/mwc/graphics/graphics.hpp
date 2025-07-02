#pragma once

#include "mwc/window/window.hpp"
#include "mwc/graphics/vulkan/context.hpp"
#include "mwc/graphics/vulkan/instance.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/surface.hpp"
#include "mwc/graphics/vulkan/queue_families.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    class graphics_ct {
      public:
      struct configuration_st {
        static constexpr auto default_configuration() -> const configuration_st;
      };
      graphics_ct(const window_ct& a_window, const semantic_version_st& a_engine_version,
                  const configuration_st& a_configuration = configuration_st::default_configuration());

      private:
      const window_ct& m_window;
      vulkan::context_st m_context;
      vulkan::instance_ct m_instance;
      vulkan::physical_device_ct m_physical_device;
      vulkan::surface_ct m_surface;
      vulkan::queue_families_ct m_queue_families;
      vulkan::logical_device_ct m_logical_device;
      configuration_st m_configuration;
    };

    // implementation
    constexpr auto graphics_ct::configuration_st::default_configuration() -> const configuration_st {
      return configuration_st {};
    }
  }
}