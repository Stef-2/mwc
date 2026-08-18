module;

export module mwc_memory_allocator;

import mwc_definition;
import mwc_vk_context;
import mwc_vk_instance;
import mwc_vk_physical_device;
import mwc_vk_logical_device;

export import vk_mem_alloc;
import vulkan;

import std;

export namespace mwc {
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

        [[nodiscard]] auto statistics() const -> vma::TotalStatistics;
        [[nodiscard]] auto budget() const -> vector_t<vma::Budget>;
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