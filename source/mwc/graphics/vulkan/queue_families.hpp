#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/surface.hpp"

import mwc_definition;
import mwc_type_mobility;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class queue_families_ct : public irreproducible_st {
        public:
        using queue_family_count_t = uint32_t;

        struct family_st {
          using index_t = uint32_t;
          using priority_t = float32_t;

          index_t m_index;
          priority_t m_priority;
        };

        struct configuration_st {
          static constexpr auto default_configuration() -> configuration_st;

          family_st::priority_t m_graphics;
          family_st::priority_t m_present;
          family_st::priority_t m_compute;
          family_st::priority_t m_transfer;
        };

        struct properties_st {
          using properties_chain_t = vk::StructureChain<vk::QueueFamilyProperties2>;

          vector_t<properties_chain_t> m_properties;
        };

        queue_families_ct(const physical_device_ct& a_physical_device, const surface_ct& a_surface,
                          const configuration_st& a_configuration = configuration_st::default_configuration());

        template <typename tp_this>
        [[nodiscard]] auto graphics(this tp_this&& a_this) -> decltype(auto);
        template <typename tp_this>
        [[nodiscard]] auto present(this tp_this&& a_this) -> decltype(auto);
        template <typename tp_this>
        [[nodiscard]] auto compute(this tp_this&& a_this) -> decltype(auto);
        template <typename tp_this>
        [[nodiscard]] auto transfer(this tp_this&& a_this) -> decltype(auto);
        template <typename tp_this>
        [[nodiscard]] auto properties(this tp_this&& a_this) -> decltype(auto);
        template <typename tp_this>
        [[nodiscard]] auto configuration(this tp_this&& a_this) -> decltype(auto);

        [[nodiscard]] auto supports_combined_graphics_and_present_family() const -> bool_t;
        [[nodiscard]] auto supports_dedicated_compute_family() const -> bool_t;
        [[nodiscard]] auto supports_dedicated_transfer_family() const -> bool_t;

        private:
        family_st m_graphics;
        family_st m_present;
        family_st m_compute;
        family_st m_transfer;

        bool_t m_combined_graphics_and_present_family;
        bool_t m_dedicated_compute_family;
        bool_t m_dedicated_transfer_family;

        properties_st m_properties;
        configuration_st m_configuration;
      };
      // implementation
      constexpr auto queue_families_ct::configuration_st::default_configuration() -> configuration_st {
        constexpr auto default_queue_family_priority = float32_t {1.0};

        return configuration_st {.m_graphics = default_queue_family_priority,
                                 .m_present = default_queue_family_priority,
                                 .m_compute = default_queue_family_priority,
                                 .m_transfer = default_queue_family_priority};
      }
      template <typename tp_this>
      auto queue_families_ct::properties(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_properties);
      }
      template <typename tp_this>
      auto queue_families_ct::graphics(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_graphics);
      }
      template <typename tp_this>
      auto queue_families_ct::present(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_present);
      }
      template <typename tp_this>
      auto queue_families_ct::compute(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_compute);
      }
      template <typename tp_this>
      auto queue_families_ct::transfer(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_transfer);
      }
      template <typename tp_this>
      auto queue_families_ct::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }
    }
  }
}