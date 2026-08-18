module;

export module mwc_vk_handle;

import mwc_definition;
import mwc_pointer_cast;
import mwc_type_mobility;
import mwc_vk_null_handle;
import mwc_vk_debug_name;

import vulkan;

import std;

export namespace mwc {
  namespace graphics {
    namespace vulkan {
      template <typename tp_vulkan_handle>
      class handle_ct : public irreproducible_st {
        public:
        using handle_t = tp_vulkan_handle;

        handle_ct(const nullptr_t a_nullptr = nullptr) noexcept;
        handle_ct(handle_t&& a_handle) noexcept /* pre(*a_handle)*/;

        template <typename tp_this>
        auto operator*(this tp_this&& a_this) -> decltype(auto) post(r : r != null_handle);
        template <typename tp_this>
        auto operator->(this tp_this&& a_this) -> decltype(auto) post(r : r != null_handle);
        template <typename tp_this>
        [[nodiscard]] auto unique_handle(this tp_this&& a_this) -> decltype(auto) post(r : r != null_handle);
        template <typename tp_this>
        [[nodiscard]] auto native_handle(this tp_this&& a_this) -> decltype(auto) post(r : r != null_handle);

        [[nodiscard]] auto handle_id() const -> uintptr_t;
        auto debug_name(const string_view_t a_string_view) const -> void;

        protected:
        handle_t m_vulkan_handle;
      };

      // implementation
      template <typename tp_vulkan_handle>
      handle_ct<tp_vulkan_handle>::handle_ct(const nullptr_t a_nullptr) noexcept
      : m_vulkan_handle(std::forward<const nullptr_t>(a_nullptr)) {}
      template <typename tp_vulkan_handle>
      handle_ct<tp_vulkan_handle>::handle_ct(tp_vulkan_handle&& a_handle) noexcept : m_vulkan_handle(std::move(a_handle)) {}
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::operator*(this auto&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_vulkan_handle);
      }
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::operator->(this auto&& a_this) -> decltype(auto) {
        return std::addressof(std::forward_like<decltype(a_this)>(a_this.m_vulkan_handle));
      }
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::unique_handle(this auto&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_vulkan_handle);
        //return m_vulkan_handle;
      }
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::native_handle(this auto&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(*a_this.m_vulkan_handle);
      }
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::handle_id() const -> uintptr_t {
        return pointer_cast(std::addressof(**m_vulkan_handle));
      }
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::debug_name(const string_view_t a_string_view) const -> void {
        mwc::graphics::vulkan::debug_name(m_vulkan_handle, a_string_view);
      }
    }
  }
}