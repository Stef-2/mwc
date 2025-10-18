#include "mwc/graphics/vulkan/command_pool.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      command_pool_ct::command_pool_ct(const logical_device_ct& a_logical_device,
                                       const queue_families_ct& a_queue_families,
                                       const configuration_st& a_configuration)
      : handle_ct {std::invoke([this, &a_logical_device, &a_queue_families, &a_configuration] -> handle_t {
          // determine if the queue flags contain more than one queue family
          const auto pop_count = std::popcount(
            static_cast<typename decltype(a_configuration.m_queue_flags)::MaskType>(a_configuration.m_queue_flags));

          m_queue_family_index = queue_families_ct::family_st::index_t {0};
          // more than one queue family requested, search for appropriate index
          if (pop_count > 1) {
            for (auto i = 0; const auto& queue_family_properties : a_queue_families.properties().m_properties) {
              const auto& properties = queue_family_properties.get<vk::QueueFamilyProperties2>();
              if (a_configuration.m_queue_flags bitand properties.queueFamilyProperties.queueFlags) {
                m_queue_family_index = i;
                break;
              }
              ++i;
            }
          }
          // specialized queue family requested, search in order from less to more specialized families
          else {
            const auto flag_bit = vk::QueueFlagBits {
              static_cast<typename decltype(a_configuration.m_queue_flags)::MaskType>(a_configuration.m_queue_flags)};

            switch (flag_bit) {
              case vk::QueueFlagBits::eTransfer : m_queue_family_index = a_queue_families.transfer().m_index; break;
              case vk::QueueFlagBits::eCompute : m_queue_family_index = a_queue_families.compute().m_index; break;
              case vk::QueueFlagBits::eGraphics : m_queue_family_index = a_queue_families.graphics().m_index; break;
              default : contract_assert(false); std::unreachable();
            }
          }

          auto expected = a_logical_device->createCommandPool(
            vk::CommandPoolCreateInfo {a_configuration.m_command_pool_create_flags, m_queue_family_index});
          contract_assert(expected.result == vk::Result::eSuccess);

          return std::move(expected.value);
        })},
        // m_queue_family_index set by the handle constructor
        m_configuration {a_configuration} {}

      auto command_pool_ct::queue_family_index() const -> queue_families_ct::family_st::index_t {
        return m_queue_family_index;
      }

    }
  }
}