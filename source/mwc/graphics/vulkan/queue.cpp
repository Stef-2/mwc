#include "mwc/graphics/vulkan/queue.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      queue_ct::queue_ct(const logical_device_ct& a_logical_device, const configuration_st& a_configuration)
      // deferred construction
      : handle_ct {std::invoke([&a_logical_device, &a_configuration] -> handle_t {
          const auto queue_index = 0;
          information(std::format("initializing vulkan device queue" SUB "queue family index: {0}" SUB "queue index: {1}",
                                  a_configuration.m_family_index, queue_index));
          auto queue = a_logical_device->getQueue2(
            vk::DeviceQueueInfo2 {vk::DeviceQueueCreateFlags {}, a_configuration.m_family_index, queue_index});

          return std::move(queue);
        })},
        m_logical_device {a_logical_device},
        //m_command_pool {a_logical_device, a_queue_families, a_configuration.m_command_pool_configuration},
        /*m_command_buffers {std::invoke([this, &a_logical_device, &a_configuration] {
          information(std::format("allocating vulkan command buffers from command pool: {0}" SUB "command buffer count: {1}",
                                  m_command_pool.handle_id(),
                                  a_configuration.m_command_buffer_count));

          auto expected = a_logical_device->allocateCommandBuffers(vk::CommandBufferAllocateInfo {
            m_command_pool.native_handle(), vk::CommandBufferLevel::ePrimary, a_configuration.m_command_buffer_count});
          contract_assert(expected);
          return std::move(expected.value());
        })},*/
        m_queue_state {queue_state_et::e_initial},
        //m_submit_fence {a_logical_device->createFence(vk::FenceCreateInfo {vk::FenceCreateFlags {}}).value()},
        m_configuration {a_configuration} {}
    }
  }
}