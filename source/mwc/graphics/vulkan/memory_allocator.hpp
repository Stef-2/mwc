#pragma once

#include "mwc/graphics/vulkan/context.hpp"
#include "mwc/graphics/vulkan/instance.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"

import vulkan_hpp;
import vk_mem_alloc_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class memory_allocator_ct : public vma::Allocator {
        public:
        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;
        };

        memory_allocator_ct(const context_st& a_context, const instance_ct& a_instance,
                            const physical_device_ct& a_physical_device, const logical_device_ct& a_logical_device,
                            const configuration_st& a_configuration = configuration_st::default_configuration());
        ~memory_allocator_ct();

        [[nodiscard]] auto statistics() const -> const vma::TotalStatistics;
        [[nodiscard]] auto budget() const -> const vector_t<vma::Budget>;
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto memory_allocator_ct::configuration_st::default_configuration() -> configuration_st {

        return configuration_st {};
      }
      template <typename tp_this>
      auto memory_allocator_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}