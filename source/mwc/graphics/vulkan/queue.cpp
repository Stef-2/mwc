#include "mwc/graphics/vulkan/queue.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      queue_ct::queue_ct(const logical_device_ct& a_logical_device, const queue_families_ct& a_queue_families,
                         const configuration_st& a_configuration)
      // deferred construction
      : handle_ct {nullptr},
        m_logical_device {a_logical_device},
        m_command_pool {a_logical_device, a_queue_families, a_configuration.m_command_pool_configuration},
        m_command_buffers {std::invoke([this, &a_logical_device, &a_configuration] {
          information(std::format("allocating vulkan command buffers from command pool: {0}\ncommand buffer count: {1}",
                                  m_command_pool.id(),
                                  a_configuration.m_command_buffer_count));

          auto expected = a_logical_device->allocateCommandBuffers(vk::CommandBufferAllocateInfo {
            m_command_pool.native_handle(), vk::CommandBufferLevel::ePrimary, a_configuration.m_command_buffer_count});
          contract_assert(expected);
          return std::move(expected.value());
        })},
        m_queue_state {queue_state_et::e_initial},
        m_submit_fence {a_logical_device->createFence(vk::FenceCreateInfo {vk::FenceCreateFlags {}}).value()},
        m_configuration {a_configuration} {
        const auto queue_family_index = m_command_pool.queue_family_index();
        const auto queue_index = 0;
        information(std::format("initializing vulkan device queue\nqueue family index: {0}\nqueue index: {1}",
                                queue_family_index, queue_index));
        auto expected =
          m_logical_device->getQueue2(vk::DeviceQueueInfo2 {vk::DeviceQueueCreateFlags {}, queue_family_index, queue_index});
        contract_assert(expected);

        this->m_vulkan_handle = std::move(expected.value());
      }
    }
  }
}