#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace {
  auto generate_device_queue_information(const mwc::graphics::vulkan::queue_families_ct& a_queue_families) {
    static constexpr auto queue_count = mwc::graphics::vulkan::queue_families_ct::queue_family_count_t {1};

    auto buffer = mwc::string_t {"initializing device queues:" SUB ""};
    constexpr auto queue_family_educated_guess_count = 4;
    buffer.reserve(buffer.size() +
                   queue_family_educated_guess_count * mwc::string_view_t {"generating x queues for family_count_name"}.size());

    auto queue_family_count = 0;
    // graphics queue is mandatory
    auto queues = mwc::vector_t<vk::DeviceQueueCreateInfo> {vk::DeviceQueueCreateInfo {
      vk::DeviceQueueCreateFlags {}, a_queue_families.graphics().m_index, queue_count, &a_queue_families.graphics().m_priority}};
    std::format_to(std::back_inserter(buffer), "[{0}] graphics queue:" SUB "queue index: {1}" SUB "queue count: {2}" SUB "",
                   queue_family_count++, a_queue_families.graphics().m_index, queue_count);
    // in case the device does not support combined graphics and present queue family
    // generate a presentation specific queue
    if (not a_queue_families.supports_combined_graphics_and_present_family()) {
      queues.emplace_back(vk::DeviceQueueCreateFlags {}, a_queue_families.present().m_index, queue_count,
                          &a_queue_families.present().m_priority);
      std::format_to(std::back_inserter(buffer),
                     "selected device does not support combined graphics and present queues, generating:" SUB "[{0}] present "
                     "queue:" SUB "queue index: {1}" SUB "queue count: {2}" SUB "",
                     queue_family_count++, a_queue_families.present().m_index, queue_count);
    }
    // dedicated compute queue
    if (a_queue_families.supports_dedicated_compute_family()) {
      queues.emplace_back(vk::DeviceQueueCreateFlags {}, a_queue_families.compute().m_index, queue_count,
                          &a_queue_families.compute().m_priority);
      std::format_to(std::back_inserter(buffer),
                     "selected device supports dedicated compute queue, generating:" SUB "[{0}] compute queues:" SUB
                     "queue index: {1}" SUB "queue count: {2}" SUB "",
                     queue_family_count++, a_queue_families.compute().m_index, queue_count);
    }
    // dedicated transfer queue
    if (a_queue_families.supports_dedicated_transfer_family()) {
      queues.emplace_back(vk::DeviceQueueCreateFlags {},
                          a_queue_families.transfer().m_index,
                          queue_count,
                          &a_queue_families.transfer().m_priority);
      std::format_to(std::back_inserter(buffer),
                     "selected device supports dedicated transfer queue, generating:" SUB "[{0}] transfer queues:" SUB
                     "queue index: {1}" SUB "queue count: {2}",
                     queue_family_count++, a_queue_families.transfer().m_index, queue_count);
    }
    mwc::information(buffer);

    return queues;
  }
}
namespace mwc {
  namespace graphics {
    namespace vulkan {
      logical_device_ct::logical_device_ct(const physical_device_ct& a_physical_device,
                                           const queue_families_ct& a_queue_families, const configuration_st& a_configuration)
      : handle_ct {std::invoke([&a_physical_device, &a_queue_families, &a_configuration] -> handle_t {
          auto buffer = string_t {"initializing vulkan logical device:" SUB "required extensions:"};
          buffer.reserve(buffer.size() + a_configuration.m_required_extensions.size() *
                                           string_view_t {"[x] VK_KHR_AverageExtensionName"}.size());

          for (auto i = 0; const auto& extension : a_configuration.m_required_extensions) {
            const auto promoted_extension = vk::isPromotedExtension(extension);
            std::format_to(std::back_inserter(buffer), "" SUB "[{0}] {1}", i++, extension);
            if (promoted_extension)
              std::format_to(std::back_inserter(buffer), " (promoted to: {0})", vk::getExtensionPromotedTo(extension));
          }

          information(buffer);

          // generate device queue information
          const auto device_queue_information = generate_device_queue_information(a_queue_families);
          // assert that the device supports all of the required extensions
          const auto available_extensions = a_physical_device->enumerateDeviceExtensionProperties();
          for (const auto& required_extension : a_configuration.m_required_extensions) {
            auto required_extensions_supported = false;
            for (const auto& available_extension : available_extensions)
              if (std::strcmp(available_extension.extensionName, required_extension) == 0) {
                required_extensions_supported = true;
                break;
              }
            if (not required_extensions_supported)
              error(std::format("required device extension: [{0}] is not available", required_extension));
          }

          const auto& device_features = a_physical_device.features();
          const auto logical_device_create_info =
            vk::DeviceCreateInfo {vk::DeviceCreateFlags {},
                                  device_queue_information,
                                  {/* device layers are deprecated */},
                                  a_configuration.m_required_extensions,
                                  {/* features field is deprecated */},
                                  std::addressof(device_features.m_default_features_chain.get<vk::PhysicalDeviceFeatures2>())};
          const auto logical_device = a_physical_device->createDevice(logical_device_create_info).value();
          return handle_t {a_physical_device->createDevice(logical_device_create_info).value()};
        })},
        m_configuration {a_configuration} {}
    }
  }
}