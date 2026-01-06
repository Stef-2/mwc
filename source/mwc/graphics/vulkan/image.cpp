#include "mwc/graphics/vulkan/image.hpp"

namespace mwc {
  namespace graphics {
    namespace vulkan {
      image_ct::image_ct(const logical_device_ct& a_logical_device, const memory_allocator_ct& a_memory_allocator,
                         const configuration_st& a_configuration)
      : handle_ct {nullptr},
        m_allocation_info {},
        m_allocation {},
        m_configuration {a_configuration} {
        auto expected = a_memory_allocator.createImage(m_configuration.m_image_create_info,
                                                       m_configuration.m_allocation_create_info, m_allocation_info);
        contract_assert(expected.result == vk::Result::eSuccess);
        auto& [allocation, image] = expected.value;

        m_vulkan_handle = handle_t {a_logical_device.unique_handle(), image};
      }
      auto image_ct::record_layout_transition(const vk::raii::CommandBuffer& a_command_buffer,
                                              const layout_transition_configuration_st& a_layout_transition_configuration) const
        -> void {
        const auto image_memory_barrier = vk::ImageMemoryBarrier2 {a_layout_transition_configuration.m_source_pipeline_stage,
                                                                   a_layout_transition_configuration.m_source_access_flags,
                                                                   a_layout_transition_configuration.m_destination_pipeline_stage,
                                                                   a_layout_transition_configuration.m_destination_access_flags,
                                                                   a_layout_transition_configuration.m_old_layout,
                                                                   a_layout_transition_configuration.m_new_layout,
                                                                   a_layout_transition_configuration.m_source_queue_family,
                                                                   a_layout_transition_configuration.m_destination_queue_family,
                                                                   native_handle(),
                                                                   a_layout_transition_configuration.m_subresource_range};

        const auto dependency_info = vk::DependencyInfo {{}, {}, {}, image_memory_barrier};

        a_command_buffer.pipelineBarrier2(dependency_info);
      }
    }
  }
}