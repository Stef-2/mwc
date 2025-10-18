#pragma once

#include "mwc/core/contract/definition.hpp"

import mwc_definition;
import mwc_contract_assertion;
import mwc_concept;

import std;

namespace mwc {
  // static storage unordered bidirectional map
  // suitable for use in constant evaluation contexts
  template <typename tp_key, typename tp_value, size_t tp_size>
  struct static_unordered_bi_map_st {
    using key_t = tp_key;
    using const_key_t = std::add_const_t<key_t>;
    using value_t = tp_value;
    using const_value_t = std::add_const_t<value_t>;
    using kv_pair_t = pair_t<key_t, value_t>;
    using storage_t = array_t<kv_pair_t, tp_size>;

    constexpr static_unordered_bi_map_st(const span_t<kv_pair_t, tp_size> a_span) pre(contract::validate_storage(a_span));
    constexpr static_unordered_bi_map_st(const initializer_list_t<kv_pair_t> a_initializer_list = {}) : m_storage {} {
      std::ranges::copy(a_initializer_list, m_storage.begin());
    }
    template <typename tp>
    [[nodiscard]] constexpr auto operator[](this tp&& a_this, const auto& a_value) -> decltype(auto)
      requires concepts::any_of_c<std::remove_cvref_t<decltype(a_value)>, key_t, value_t>;

    storage_t m_storage;
  };

  // explicit deduction guide
  template <typename tp_key, typename tp_value, size_t tp_size>
  static_unordered_bi_map_st(const pair_t<tp_key, tp_value> (&)[tp_size])
    -> static_unordered_bi_map_st<tp_key, tp_value, tp_size>;

  // implementation
  template <typename tp_key, typename tp_value, size_t tp_size>
  constexpr static_unordered_bi_map_st<tp_key, tp_value, tp_size>::static_unordered_bi_map_st(
    const span_t<kv_pair_t, tp_size> a_span)
  : m_storage {} {
    std::ranges::copy(a_span, m_storage.begin());
  }
  template <typename tp_key, typename tp_value, size_t tp_size>
  [[nodiscard]] constexpr auto
  static_unordered_bi_map_st<tp_key, tp_value, tp_size>::operator[](this auto&& a_this, const auto& a_value) -> decltype(auto)
    requires concepts::any_of_c<std::remove_cvref_t<decltype(a_value)>, key_t, value_t> {
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(a_value)>, key_t>) {
      for (const auto& kv_pair : a_this.m_storage)
        if (kv_pair.first == a_value)
          return std::forward_like<decltype(a_value)>(kv_pair.second);
    }

    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(a_value)>, value_t>) {
      for (const auto& kv_pair : a_this.m_storage)
        if (kv_pair.second == a_value)
          return std::forward_like<decltype(a_value)>(kv_pair.first);
    }

    contract_assert(false);
    std::unreachable();
  }
}