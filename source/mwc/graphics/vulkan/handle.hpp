#pragma once

import mwc_definition;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      template <typename tp_vulkan_handle>
      class handle_ct {
        public:
        using handle_t = tp_vulkan_handle;

        handle_ct(const nullptr_t a_nullptr = nullptr);
        handle_ct(handle_t&& a_handle) /* pre(*a_handle)*/;
        handle_ct(const handle_ct&) = delete("move only type");
        auto operator=(const handle_ct&) -> handle_ct& = delete("move only type");

        // note: change these preconditions into postconditions once the contract implementation allows it
        template <typename tp_this>
        auto operator*(this tp_this&& a_this) -> decltype(auto) pre(*a_this.m_vulkan_handle);
        template <typename tp_this>
        auto operator->(this tp_this&& a_this) -> decltype(auto) pre(*a_this.m_vulkan_handle);
        template <typename tp_this>
        auto unique_handle(this tp_this&& a_this) -> decltype(auto) pre(*a_this.m_vulkan_handle);
        template <typename tp_this>
        auto native_handle(this tp_this&& a_this) -> decltype(auto) pre(*a_this.m_vulkan_handle);

        private:
        handle_t m_vulkan_handle;
      };

      // implementation
      template <typename tp_vulkan_handle>
      handle_ct<tp_vulkan_handle>::handle_ct(const nullptr_t a_nullptr)
      : m_vulkan_handle(std::forward<const nullptr_t>(a_nullptr)) {}
      template <typename tp_vulkan_handle>
      handle_ct<tp_vulkan_handle>::handle_ct(tp_vulkan_handle&& a_handle) : m_vulkan_handle(std::move(a_handle)) {}
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::operator*(this auto&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_vulkan_handle);
      }
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::operator->(this auto&& a_this) -> decltype(auto) {
        return &std::forward_like<decltype(a_this)>(a_this.m_vulkan_handle);
      }
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::unique_handle(this auto&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_vulkan_handle);
      }
      template <typename tp_vulkan_handle>
      auto handle_ct<tp_vulkan_handle>::native_handle(this auto&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(*a_this.m_vulkan_handle);
      }
    }
  }
}