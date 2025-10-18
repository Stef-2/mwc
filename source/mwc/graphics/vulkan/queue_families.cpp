#include "mwc/graphics/vulkan/queue_families.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      queue_families_ct::queue_families_ct(const physical_device_ct& a_physical_device, const surface_ct& a_surface,
                                           const configuration_st& a_configuration)
      : m_graphics {std::numeric_limits<family_st::index_t>::max(), a_configuration.m_graphics},
        m_present {std::numeric_limits<family_st::index_t>::max(), a_configuration.m_present},
        m_compute {std::numeric_limits<family_st::index_t>::max(), a_configuration.m_compute},
        m_transfer {std::numeric_limits<family_st::index_t>::max(), a_configuration.m_transfer},
        m_combined_graphics_and_present_family {false},
        m_dedicated_compute_family {false},
        m_dedicated_transfer_family {false},
        m_properties {std::invoke([&a_physical_device] -> properties_st {
          return properties_st {a_physical_device->getQueueFamilyProperties2<properties_st::properties_chain_t>()};
        })},
        m_configuration {a_configuration} {
        for (auto i = family_st::index_t {0}; const auto& queue_family_property : m_properties.m_properties) {
          const auto& queue_flags = queue_family_property.get<vk::QueueFamilyProperties2>().queueFamilyProperties.queueFlags;
          if (queue_flags bitand vk::QueueFlagBits::eGraphics)
            m_graphics.m_index = i;
          if (queue_flags bitand vk::QueueFlagBits::eCompute)
            m_compute.m_index = i;
          if (queue_flags bitand vk::QueueFlagBits::eTransfer)
            m_transfer.m_index = i;
          ++i;
        }
        for (auto i = family_st::index_t {0}; i < std::numeric_limits<family_st::index_t>::max(); ++i)
          if (a_physical_device->getSurfaceSupportKHR(i, a_surface.native_handle()).value) {
            m_present.m_index = i;
            break;
          }
        if (m_graphics.m_index == m_present.m_index)
          m_combined_graphics_and_present_family = true;
        if (m_graphics.m_index != m_compute.m_index)
          m_dedicated_compute_family = true;
        if (m_graphics.m_index != m_transfer.m_index)
          m_dedicated_transfer_family = true;
      }

      auto queue_families_ct::supports_combined_graphics_and_present_family() const -> bool_t {
        return m_combined_graphics_and_present_family;
      }
      auto queue_families_ct::supports_dedicated_compute_family() const -> bool_t {
        return m_dedicated_compute_family;
      }
      auto queue_families_ct::supports_dedicated_transfer_family() const -> bool_t {
        return m_dedicated_transfer_family;
      }
    }
  }
}