#include "mwc/graphics/vulkan/instance.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

import vkfw;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      instance_ct::instance_ct(const context_st& a_context, const configuration_st& a_configuration)
      : handle_ct {std::invoke([&a_context, &a_configuration] -> handle_ct::handle_t {
          auto buffer = string_t {};

          std::format_to(std::back_inserter(buffer),
                         "creating a vulkan instance:\nrequired version: {0}",
                         string_t {a_context.m_vulkan_api_version});

          if (not a_configuration.m_required_layers.empty()) {
            std::format_to(std::back_inserter(buffer), "\nrequired layers:");
            for (auto i = 0; const auto& layer : a_configuration.m_required_layers)
              std::format_to(std::back_inserter(buffer), "\n[{0}] {1}", i, layer);
          }

          // combine the requested extensions with the ones required by the window system integration
          const auto wsi_extensions = vkfw::getRequiredInstanceExtensions();
          auto combined_extensions = vector_t<const char_t*> {};
          combined_extensions.reserve(wsi_extensions.size() + a_configuration.m_required_extensions.size());
          std::format_to(std::back_inserter(buffer), "\nrequired extensions:");
          for (const auto& wsi_extenison : wsi_extensions) {
            std::format_to(std::back_inserter(buffer), "\n[{0}] {1}", combined_extensions.size(), wsi_extenison);
            combined_extensions.emplace_back(wsi_extenison);
          }
          for (const auto& extension : a_configuration.m_required_extensions) {
            std::format_to(std::back_inserter(buffer), "\n[{0}] {1}", combined_extensions.size(), extension);
            combined_extensions.emplace_back(extension);
          }
          // assert that the instance supports all of the required extensions
          const auto [_, available_extensions] = vk::enumerateInstanceExtensionProperties();
          for (const auto& required_extension : combined_extensions) {
            auto required_extensions_supported = false;
            for (const auto& available_extension : available_extensions)
              if (std::strcmp(available_extension.extensionName, required_extension) == 0) {
                required_extensions_supported = true;
                break;
              }
            if (not required_extensions_supported)
              error(std::format("required instance extension: [{0}] is not available", required_extension));
          }

          information(buffer);

          const auto& engine_version = a_context.m_engine_version.m_version;
          const auto& vulkan_api_version = a_context.m_vulkan_api_version.m_version;
          const auto application_info = vk::ApplicationInfo {project_name_string().data(), engine_version,
                                                             project_name_string().data(), engine_version, vulkan_api_version};
          const auto instance_create_info = vk::InstanceCreateInfo {vk::InstanceCreateFlags {}, &application_info,
                                                                    a_configuration.m_required_layers, combined_extensions};
          const auto [result, instance] = vk::createInstance(instance_create_info);
          return vk::raii::Instance {a_context.m_context, instance};
        })} {}
    }
  }
}