#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/core/utility/semantic_version.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      physical_device_ct::physical_device_ct(const instance_ct& a_instance, const configuration_st& a_configuration)
      : handle_ct {std::invoke([&a_instance] {
          information("enumerating available physical devices");
          const auto physical_devices = a_instance->enumeratePhysicalDevices().value;
          contract_assert(not physical_devices.empty());

          // attempt to find and select a discrete gpu
          // if none can be found, select the first available one
          auto physical_device_index = 0;
          auto physical_device_properties = vk::PhysicalDeviceProperties {};
          for (auto i = 0uz; i < physical_devices.size(); ++i) {
            physical_device_properties = physical_devices[i].getProperties();
            if (physical_device_properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
              physical_device_index = i;
              break;
            }
          }
          if (physical_device_index == 0)
            physical_device_properties = physical_devices[0].getProperties();

          const auto device_name
            = string_view_t {physical_device_properties.deviceName, std::strlen(physical_device_properties.deviceName)};
          information(std::format("selecting physical device {0}" SUB "vulkan api version: {1}",
                                  device_name,
                                  string_t {semantic_version_st {physical_device_properties.apiVersion}}));
          return physical_devices[physical_device_index];
        })},
        m_properties {std::invoke([this]([[maybe_unused]] this auto&& a_this) -> properties_st {
          // structure chain type deduction
          [[maybe_unused]] auto& [... default_properties_pack] = m_properties.m_default_properties_chain;
          [[maybe_unused]] auto& [... memory_properties_pack] = m_properties.m_memory_properties_chain;

          return properties_st {(*this)->getProperties2<decltype(default_properties_pack)...>(),
                                (*this)->getMemoryProperties2<decltype(memory_properties_pack)...>(),
                                (*this)->getQueueFamilyProperties2<properties_st::queue_family_chain_t>()};
        })},
        m_features {std::invoke([this]([[maybe_unused]] this auto&& a_this) -> features_st {
          // structure chain type deduction
          [[maybe_unused]] auto& [... default_features_pack] = m_features.m_default_features_chain;

          return features_st {(*this)->getFeatures2<decltype(default_features_pack)...>()};
        })},
        m_configuration {a_configuration} {}
    }
  }
}
