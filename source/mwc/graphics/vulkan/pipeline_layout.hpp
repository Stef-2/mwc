#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/graphics/vulkan/handle.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class pipeline_layout_ct : public handle_ct<vk::raii::PipelineLayout> {
        public:
        struct configuration_st {
          using descriptor_count_t = uint32_t;
          static constexpr auto default_configuration() -> configuration_st;

          descriptor_count_t m_combined_image_sampler_count;
        };

        pipeline_layout_ct(const physical_device_ct& a_physical_device, const logical_device_ct& a_logical_device,
                           const configuration_st& a_configuration = configuration_st::default_configuration());

        [[nodiscard]] auto combined_image_sampler_layout() const -> const vk::raii::DescriptorSetLayout&;
        [[nodiscard]] auto push_constant_range() const -> const vk::PushConstantRange&;
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        vk::raii::DescriptorSetLayout m_combined_image_sampler_layout;
        vk::PushConstantRange m_push_constant_range;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto pipeline_layout_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {.m_combined_image_sampler_count = 4096};
      }
      template <typename tp_this>
      auto pipeline_layout_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}