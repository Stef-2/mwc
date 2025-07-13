#include "mwc/graphics/vulkan/pipeline_layout.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"

namespace {
  auto clamp_descriptor_count(
    const mwc::graphics::vulkan::physical_device_ct& a_physical_device,
    const mwc::graphics::vulkan::pipeline_layout_ct::configuration_st::descriptor_count_t a_requested_descriptor_count) {
    const auto descriptor_indexing_properties =
      a_physical_device.properties().m_default_properties_chain.get<vk::PhysicalDeviceDescriptorIndexingProperties>();

    return std::min(a_requested_descriptor_count, descriptor_indexing_properties.maxDescriptorSetUpdateAfterBindSampledImages);
  }
}

namespace mwc {
  namespace graphics {
    namespace vulkan {
      pipeline_layout_ct::pipeline_layout_ct(const physical_device_ct& a_physical_device,
                                             const logical_device_ct& a_logical_device, const configuration_st& a_configuration)
      // requires other members for initialization
      : handle_ct {nullptr},
        m_combined_image_sampler_layout {
        std::invoke([&a_physical_device, &a_logical_device, &a_configuration] -> decltype(m_combined_image_sampler_layout) {
          const auto descriptor_indexing_flags = vk::DescriptorBindingFlags {
            vk::DescriptorBindingFlagBits::ePartiallyBound bitor vk::DescriptorBindingFlagBits::eUpdateUnusedWhilePending};
          const auto descriptor_indexing_bindings = vk::DescriptorSetLayoutBindingFlagsCreateInfo {descriptor_indexing_flags};

          const auto combined_image_sampler_binding =
            vk::DescriptorSetLayoutBinding {0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eAll};

          auto expected = a_logical_device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo {
            vk::DescriptorSetLayoutCreateFlags {}, combined_image_sampler_binding, &descriptor_indexing_bindings});
          contract_assert(expected);

          return std::move(expected.value());
        })},
        m_push_constant_range {vk::ShaderStageFlagBits::eAll, /*offset*/ 0,
                               a_physical_device.properties()
                                 .m_default_properties_chain.get<vk::PhysicalDeviceProperties2>()
                                 .properties.limits.maxPushConstantsSize},
        m_configuration {.m_combined_image_sampler_count =
                           clamp_descriptor_count(a_physical_device, a_configuration.m_combined_image_sampler_count)} {
        information(std::format("initializing vulkan pipeline layout:" SUB "combined image sampler descriptor count: {0}" SUB
                                "push constant range size: {1} bytes",
                                m_configuration.m_combined_image_sampler_count, m_push_constant_range.size));
        auto expected = a_logical_device->createPipelineLayout(vk::PipelineLayoutCreateInfo {
          vk::PipelineLayoutCreateFlags {}, *m_combined_image_sampler_layout, m_push_constant_range});
        contract_assert(expected);

        m_vulkan_handle = {std::move(expected.value())};
      }
      auto pipeline_layout_ct::combined_image_sampler_layout() const -> const vk::raii::DescriptorSetLayout& {
        return m_combined_image_sampler_layout;
      }
      auto pipeline_layout_ct::push_constant_range() const -> const vk::PushConstantRange& {
        return m_push_constant_range;
      }
    }
  }
}
