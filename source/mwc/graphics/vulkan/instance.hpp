#pragma once

#include "mwc/core/utility/semantic_version.hpp"
#include "mwc/graphics/vulkan/context.hpp"
#include "mwc/graphics/vulkan/handle.hpp"

import mwc_definition;
import mwc_project_name_string;

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
                    const configuration_st& a_configuration = configuration_st::default_configuration());

        private:
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto instance_ct::configuration_st::default_configuration() -> configuration_st {
        static constexpr auto required_extensions = array_t {"VK_KHR_surface"};

        return configuration_st {{}, {required_extensions}};
      }
    }
  }
}