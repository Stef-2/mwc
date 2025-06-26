#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/core/utility/semantic_version.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      template <>
      physical_device_ct::physical_device_template_ct(const instance_ct& a_instance, const configuration_st& a_configuration)
      : handle_ct {std::invoke([&a_instance] {
          information("enumerating available physical devices");
          const auto physical_devices = a_instance->enumeratePhysicalDevices();
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

          const auto device_name =
            string_view_t {physical_device_properties.deviceName, std::strlen(physical_device_properties.deviceName)};
          information(std::format("selecting physical device {0}\nvulkan api version: {1}",
                                  device_name,
                                  string_t {semantic_version_st {physical_device_properties.apiVersion}}));
          return physical_devices[physical_device_index];
        })},
        m_properties {[this]<typename>() {
          const auto& vulkan_handle = static_cast<const vk::raii::PhysicalDevice&>(**this);
          const auto& [... elements] = m_properties.m_properties;
          vk::StructureChain<decltype(elements...[2])> asd;
          return std::make_tuple(
            vulkan_handle.getProperties2<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceShaderObjectPropertiesEXT>());
        }.template operator()<int>()},
        m_features {},
        m_configuration {a_configuration} {}
    }
  }
}
