#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/window/window.hpp"
#include "mwc/graphics/vulkan/context.hpp"
#include "mwc/graphics/vulkan/instance.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/surface.hpp"
#include "mwc/graphics/vulkan/queue_families.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/memory_allocator.hpp"
#include "mwc/graphics/vulkan/swapchain.hpp"
#include "mwc/graphics/vulkan/pipeline_layout.hpp"
#include "mwc/graphics/vulkan/command_pool.hpp"
#include "mwc/graphics/vulkan/frame_synchronizer.hpp"
#include "mwc/graphics/vulkan/queue.hpp"
#include "mwc/graphics/vulkan/suballocated_memory_mapped_buffer.hpp"
#include "mwc/graphics/vulkan/dynamic_rendering_state.hpp"
#include "mwc/graphics/user_interface/user_interface.hpp"
#include "mwc/graphics/camera/camera.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    class graphics_ct {
      public:
      struct configuration_st {
        static constexpr auto default_configuration() -> const configuration_st;

        uint32_t m_frames_in_flight;
      };

      graphics_ct(const window_ct& a_window, const semantic_version_st& a_engine_version,
                  const configuration_st& a_configuration = configuration_st::default_configuration());

      auto render() -> void;

      template <typename tp_this>
      [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

      private:
      const window_ct& m_window;
      vulkan::context_st m_context;
      vulkan::instance_ct m_instance;
      vulkan::physical_device_ct m_physical_device;
      vulkan::surface_ct m_surface;
      vulkan::queue_families_ct m_queue_families;
      vulkan::logical_device_ct m_logical_device;
      vulkan::memory_allocator_ct m_memory_allocator;
      vulkan::swapchain_ct m_swapchain;
      vulkan::pipeline_layout_ct m_pipeline_layout;
      vulkan::command_pool_ct m_command_pool;
      vulkan::frame_synchornizer_st<> m_frame_synchronizer;
      vulkan::queue_ct m_graphics_queue;
      vulkan::queue_ct m_transfer_queue;
      vulkan::suballocated_memory_mapped_buffer_ct m_common_buffer;
      vulkan::dynamic_rendering_state_ct m_dynamic_rendering_state;
      user_interface_ct m_user_interface;
      camera_ct m_camera;
      configuration_st m_configuration;
    };

    // implementation
    constexpr auto graphics_ct::configuration_st::default_configuration() -> const configuration_st {
      return configuration_st {.m_frames_in_flight = 2};
    }
    template <typename tp_this>
    auto graphics_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
      return std::forward_like<decltype(a_this)>(a_this.m_configuration);
    }
  }
}