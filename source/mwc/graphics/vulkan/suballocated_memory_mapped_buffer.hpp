#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/graphics/vulkan/memory_mapped_buffer.hpp"
#include "mwc/core/memory/virtual_allocator.hpp"
#include "mwc/core/container/data_span.hpp"

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class suballocated_memory_mapped_buffer_ct final : public memory_mapped_buffer_ct {
        public:
        template <typename tp>
        using suballocation_t = data_span_st<tp>;

        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          memory_mapped_buffer_ct::configuration_st m_memory_mapped_buffer_configuration;
          virtual_allocator_ct::configuration_st m_virtual_allocator_configuration;
        };

        suballocated_memory_mapped_buffer_ct(
          const logical_device_ct& a_logical_device, const memory_allocator_ct& a_memory_allocator,
          const configuration_st& a_configuration = configuration_st::default_configuration());

        template <typename tp>
        [[nodiscard]] auto request_suballocation(this auto&& a_this, size_t a_virtual_suballocation_size,
                                                 virtual_allocator_ct::allocation_configuration_st a_a_configuration =
                                                   virtual_allocator_ct::allocation_configuration_st::default_configuration())
          pre(std::modulus<decltype(a_virtual_suballocation_size)> {}(a_virtual_suballocation_size, sizeof(tp)) == 0);
        template <typename tp>
        auto release_suballocation(const suballocation_t<tp>& a_suballocation) -> void;

        [[nodiscard]] auto virtual_allocator() const -> const virtual_allocator_ct&;
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        virtual_allocator_ct m_virtual_allocator;
        configuration_st m_configuration;
      };

      // implementation
      constexpr auto suballocated_memory_mapped_buffer_ct::configuration_st::default_configuration() -> configuration_st {
        return configuration_st {
          .m_memory_mapped_buffer_configuration = memory_mapped_buffer_ct::configuration_st::default_configuration(),
          .m_virtual_allocator_configuration = virtual_allocator_ct::configuration_st::default_configuration()};
      }
      template <typename tp>
      auto suballocated_memory_mapped_buffer_ct::request_suballocation(
        this auto&& a_this, size_t a_virtual_suballocation_size,
        virtual_allocator_ct::allocation_configuration_st a_a_configuration) {
        // request a virtual suballocation, result is validated internally
        const auto virtual_allocation_offset =
          a_this.m_virtual_allocator.request_suballocation(a_virtual_suballocation_size, a_a_configuration);

        const auto physical_data_address = static_cast<byte_t*>(a_this.mapped_data_pointer()) + virtual_allocation_offset;
        const auto element_count = a_virtual_suballocation_size / sizeof(tp);

        return std::forward_like<decltype(a_this)>(suballocation_t<tp> {std::bit_cast<tp*>(physical_data_address), element_count});
      }
      template <typename tp>
      auto suballocated_memory_mapped_buffer_ct::release_suballocation(const suballocation_t<tp>& a_suballocation) -> void {
        const auto suballocation_offset =
          std::bit_cast<byte_t*>(a_suballocation.data()) - static_cast<byte_t*>(mapped_data_pointer());

        m_virtual_allocator.release_suballocation(suballocation_offset);
      }
      template <typename tp_this>
      auto suballocated_memory_mapped_buffer_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}