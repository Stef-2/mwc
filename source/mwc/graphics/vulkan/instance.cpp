#include "mwc/graphics/vulkan/instance.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      instance_ct::instance_ct(const context_st& a_context, const configuration_st& a_configuration)
      : handle_ct {std::invoke([&a_context, &a_configuration] {
          auto buffer = string_t {};

          std::format_to(std::back_inserter(buffer),
                         "creating a vulkan instance:\nrequired version: {0}",
                         string_t {a_context.m_vulkan_api_version});

          if (not a_configuration.m_required_layers.empty()) {
            std::format_to(std::back_inserter(buffer), "\nrequired layers:");
            for (auto i = 0; const auto& layer : a_configuration.m_required_layers)
              std::format_to(std::back_inserter(buffer), "\n[{0}] {1}", i, layer);
          }

          if (not a_configuration.m_required_extensions.empty()) {
            std::format_to(std::back_inserter(buffer), "\nrequired extensions:");
            for (auto i = 0; const auto& extension : a_configuration.m_required_extensions)
              std::format_to(std::back_inserter(buffer), "\n[{0}] {1}", i, extension);
          }

          information(buffer);

          const auto& engine_version = a_context.m_engine_version.m_version;
          const auto& vulkan_api_version = a_context.m_vulkan_api_version.m_version;
          const auto application_info = vk::ApplicationInfo {project_name_string().data(), engine_version,
                                                             project_name_string().data(), engine_version, vulkan_api_version};
          const auto instance_create_info =
            vk::InstanceCreateInfo {vk::InstanceCreateFlags {}, &application_info, a_configuration.m_required_layers,
                                    a_configuration.m_required_extensions};
          return vk::raii::Instance {a_context.m_context, instance_create_info};
        })} {}

    }
  }
}