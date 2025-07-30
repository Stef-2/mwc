#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/graphics/vulkan/buffer.hpp"
#include "mwc/core/utility/pointer_cast.hpp"

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      class memory_mapped_buffer_ct : public buffer_ct {
        public:
        using memory_mapped_ptr_t = void*;

        using buffer_ct::buffer_ct;

        [[nodiscard]] auto mapped_data_pointer() const -> memory_mapped_ptr_t post(r : r != memory_mapped_ptr_t {nullptr});
        auto map() -> void;
        auto unmap() -> void;

        template <typename tp>
          requires(not std::is_pointer_v<tp>)
        auto map_data(const tp& a_data, const size_t a_memory_offset = 0, const size_t a_data_size = sizeof(tp)) const
          -> void pre(a_data_size > 0) pre(a_memory_offset + a_data_size <= size());

        private:
      };

      // implementation
      template <typename tp>
        requires(not std::is_pointer_v<tp>)
      auto memory_mapped_buffer_ct::map_data(const tp& a_data, const size_t a_memory_offset, const size_t a_data_size) const
        -> void {
        const auto destination = static_cast<byte_t*>(mapped_data_pointer()) + a_memory_offset;
        contract_assert(destination);

        std::memcpy(destination, &a_data, a_data_size);
      }
    }
  }
}