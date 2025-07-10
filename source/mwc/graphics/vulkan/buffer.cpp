#include "buffer.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      buffer_ct::buffer_ct(const logical_device_ct& a_logical_device, const memory_allocator_ct& a_memory_allocator,
                           const configuration_st& a_configuration)
      : handle_ct {nullptr},
        m_logical_device {a_logical_device},
        m_memory_allocator {a_memory_allocator},
        m_device_address {0},
        m_allocation_info {},
        m_allocation {},
        m_configuration {a_configuration} {
        auto expected = a_memory_allocator.createBuffer(m_configuration.m_buffer_create_info,
                                                        m_configuration.m_allocation_create_info, m_allocation_info);
        contract_assert(expected.result == vk::Result::eSuccess);
        auto& [buffer, allocation] = expected.value;
        contract_assert(m_allocation_info.size >= m_buffer_size);
        if (device_address_requested()) {
          const auto buffer_device_address_info = vk::BufferDeviceAddressInfo {m_vulkan_handle};
          m_device_address = a_logical_device->getBufferAddress(buffer_device_address_info);
        }

        m_vulkan_handle = handle_t {a_logical_device.unique_handle(), buffer};
      }
      buffer_ct::~buffer_ct() {
        if (*m_vulkan_handle and m_allocation)
          m_memory_allocator.freeMemory(m_allocation);
      }
      auto buffer_ct::device_address_requested() const -> bool_t {
        return bool_t {m_configuration.m_buffer_create_info.usage bitand vk::BufferUsageFlagBits::eShaderDeviceAddress};
      }
      auto buffer_ct::address() const -> vk::DeviceAddress {
        return m_device_address;
      }
      auto buffer_ct::size() const -> vk::DeviceSize {
        return m_allocation_info.size;
      }
    }
  }
}