#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/utility/semantic_version.hpp"
#include "mwc/graphics/vulkan/context.hpp"
#include "mwc/graphics/vulkan/handle.hpp"

import mwc_definition;
import mwc_project_name_string;
import mwc_contract_assertion;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class instance_ct : public handle_ct<vk::raii::Instance> {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          span_t<const char_t* const> m_required_layers;
          span_t<const char_t* const> m_required_extensions;
        };

        instance_ct(const context_st& a_context,
                    const configuration_st& a_configuration = configuration_st::default_configuration())
          pre(not a_configuration.m_required_layers.empty() ? contract::validate_storage(a_configuration.m_required_layers) : true)
            pre(not a_configuration.m_required_extensions.empty()
                  ? contract::validate_storage(a_configuration.m_required_extensions)
                  : true);

        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto instance_ct::configuration_st::default_configuration() -> configuration_st {
        static constexpr auto required_extensions = array_t {
          vk::EXTDebugUtilsExtensionName, vk::KHRGetSurfaceCapabilities2ExtensionName, vk::EXTSurfaceMaintenance1ExtensionName};

        return configuration_st {.m_required_layers = {}, .m_required_extensions = required_extensions};
      }
      template <typename tp_this>
      auto instance_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}