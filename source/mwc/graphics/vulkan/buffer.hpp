#pragma once

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/memory_allocator.hpp"

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class buffer_ct : public handle_ct<vk::raii::Buffer> {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          vk::BufferCreateInfo m_buffer_create_info;
          vma::AllocationCreateInfo m_allocation_create_info;
        };
        buffer_ct(const logical_device_ct& a_logical_device, const memory_allocator_ct& a_memory_allocator,
                  const configuration_st& a_configuration = configuration_st::default_configuration());
        ~buffer_ct();

        template <typename tp_this>
        [[nodiscard]] auto allocation_info(this tp_this&& a_this) -> decltype(auto);
        template <typename tp_this>
        [[nodiscard]] auto allocation(this tp_this&& a_this) -> decltype(auto);

        [[nodiscard]] auto device_address_requested() const -> bool_t;
        [[nodiscard]] auto address() const -> vk::DeviceAddress pre(device_address_requested())
          post(r : r != vk::DeviceAddress {0});
        [[nodiscard]] auto size() const -> vk::DeviceSize post(r : r != vk::DeviceSize {0});
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        protected:
        const logical_device_ct& m_logical_device;
        const memory_allocator_ct& m_memory_allocator;

        vk::DeviceSize m_buffer_size;
        vk::DeviceAddress m_device_address;
        vma::AllocationInfo m_allocation_info;
        vma::Allocation m_allocation;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto buffer_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {
          .m_buffer_create_info = vk::BufferCreateInfo {vk::BufferCreateFlags {}, vk::DeviceSize {256},
                                                        vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive,
                                                        /*pQueueFamilyIndices*/ 0},
          .m_allocation_create_info = vma::AllocationCreateInfo {
          vma::AllocationCreateFlags {}, vma::MemoryUsage::eAuto, vk::MemoryPropertyFlags {/*requiredFlags*/},
          vk::MemoryPropertyFlags {/*preferredFlags*/ vk::MemoryPropertyFlagBits::eDeviceLocal}}};
      }
      template <typename tp_this>
      auto buffer_ct::allocation_info(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_allocation_info);
      }
      template <typename tp_this>
      auto buffer_ct::allocation(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_allocation);
      }
      template <typename tp_this>
      auto buffer_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}