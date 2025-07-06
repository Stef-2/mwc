#include "mwc/graphics/vulkan/memory_allocator.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/core/utility/semantic_version.hpp"

import mwc_observer_ptr;
import mwc_operating_system;

namespace {
  constexpr auto vma_extension_map(const vma::AllocatorCreateFlagBits a_flag_bit) -> mwc::string_t {
    using enum vma::AllocatorCreateFlagBits;
    switch (a_flag_bit) {
      case eKhrDedicatedAllocation : return {vk::KHRDedicatedAllocationExtensionName};
      case eKhrBindMemory2 : return {vk::KHRBindMemory2ExtensionName};
      case eExtMemoryBudget : return {vk::EXTMemoryBudgetExtensionName};
      case eAmdDeviceCoherentMemory : return {vk::AMDDeviceCoherentMemoryExtensionName};
      case eBufferDeviceAddress : return {vk::KHRBufferDeviceAddressExtensionName};
      case eExtMemoryPriority : return {vk::EXTMemoryPriorityExtensionName};
      case eKhrMaintenance4 : return {vk::KHRMaintenance4ExtensionName};
      case eKhrMaintenance5 :
        return {vk::KHRMaintenance5ExtensionName};
        // win32 only
      case eKhrExternalMemoryWin32 :
#ifdef _WIN32
        return {vk::KHRExternalMemoryWin32ExtensionName};
#else
        return {};
#endif
        // this flag does not map to any logica device extension
      case eExternallySynchronized : contract_assert(false); std::unreachable();
    }
  }
}

namespace mwc {
  namespace graphics {
    namespace vulkan {
      memory_allocator_ct::memory_allocator_ct(
        const context_st& a_context, const instance_ct& a_instance, const physical_device_ct& a_physical_device,
        const logical_device_ct& a_logical_device, const configuration_st& a_configuration)
      : vma::Allocator {std::invoke([&a_context, &a_instance, &a_physical_device, &a_logical_device] -> vma::Allocator {
          auto allocator_extensions = vma::AllocatorCreateFlags {};

          // full vulkan memory allocator extension bitmask
          constexpr auto allocator_flag_mask = static_cast<std::underlying_type_t<vma::AllocatorCreateFlagBits>>(
            vk::FlagTraits<vma::AllocatorCreateFlagBits>::allFlags);
          // bit to begin the iteration from
          constexpr auto allocator_begin_bit = std::to_underlying(vma::AllocatorCreateFlagBits::eExternallySynchronized) + 1;

          auto buffer = string_t {"initializing vulkan memory allocator:\nutilizing extensions:"};
          for (auto i = decltype(allocator_flag_mask) {0}, e = allocator_begin_bit; e < allocator_flag_mask; e = e << 1) {
            // extract individual extension bits and convert them to their respective device extensions
            const auto bit_flag = vma::AllocatorCreateFlagBits {e};
            const auto extension_string = vma_extension_map(bit_flag);
            if (vk::isPromotedExtension(extension_string) or
                std::ranges::contains(a_logical_device.configuration().m_required_extensions, extension_string)) {
              std::format_to(std::back_inserter(buffer), "\n[{0}] {1}", i++, extension_string);
              allocator_extensions |= bit_flag;
            }
          }
          information(buffer);

          const auto allocator_info = vma::AllocatorCreateInfo {allocator_extensions,
                                                                a_physical_device.native_handle(),
                                                                a_logical_device.native_handle(),
                                                                vk::DeviceSize {/*preferredLargeHeapBlockSize_*/ 0},
                                                                mwc::obs_ptr_t<vk::AllocationCallbacks> {nullptr},
                                                                mwc::obs_ptr_t<vma::DeviceMemoryCallbacks> {nullptr},
                                                                mwc::obs_ptr_t<vk::DeviceSize> {/*pHeapSizeLimit_*/ nullptr},
                                                                mwc::obs_ptr_t<vma::VulkanFunctions> {nullptr},
                                                                a_instance.native_handle(),
                                                                a_context.m_vulkan_api_version.m_version,
                                                                mwc::obs_ptr_t<vk::ExternalMemoryHandleTypeFlagsKHR> {nullptr}};

          auto allocator = vma::Allocator {};
          const auto result = vma::createAllocator(&allocator_info, &allocator);
          contract_assert(result == vk::Result::eSuccess);

          return allocator;
        })},
        m_configuration {a_configuration} {}
      memory_allocator_ct::~memory_allocator_ct() {
        this->destroy();
      }
      auto memory_allocator_ct::statistics() const -> const vma::TotalStatistics {
        return this->calculateStatistics();
      }
      auto memory_allocator_ct::budget() const -> const vector_t<vma::Budget> {
        return this->getHeapBudgets();
      }
    }
  }
}