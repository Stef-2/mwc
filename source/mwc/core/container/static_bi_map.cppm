module;

#include "mwc/core/contract/natural_syntax.hpp"

export module mwc_static_bi_map;

import mwc_definition;
import mwc_contract_assertion;
import mwc_concept;

import std;

export namespace mwc {
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

    constexpr static_unordered_bi_map_st(const span_t<kv_pair_t, tp_size> a_span)
      pre(contract::validate_data_size(a_span))
    : m_storage {} {
      std::ranges::copy(a_span, m_storage.begin());
    }

    template <typename tp>
    [[nodiscard]] constexpr auto operator[](this tp&& a_this, const auto a_value)
      -> decltype(auto) pre(contract::validate_data_size(a_this.m_storage))
      requires concepts::any_of_c<decltype(a_value), const_key_t, const_value_t>;

    storage_t m_storage;
  };

  // explicit deduction guide
  template <typename tp_key, typename tp_value, size_t tp_size>
  static_unordered_bi_map_st(const pair_t<tp_key, tp_value> (&)[tp_size])
    -> static_unordered_bi_map_st<tp_key, tp_value, tp_size>;

  // implementation
  template <typename tp_key, typename tp_value, size_t tp_size>
  [[nodiscard]] constexpr auto
  static_unordered_bi_map_st<tp_key, tp_value, tp_size>::operator[](
    this auto&& a_this,
    const auto a_value) -> decltype(auto)
    requires concepts::any_of_c<decltype(a_value), const_key_t, const_value_t>
  {
    if constexpr (std::is_same_v<decltype(a_value), const_key_t>) {
      for (const auto& kv_pair : a_this.m_storage)
        if (kv_pair.first == a_value)
          return kv_pair.second;
    }

    if constexpr (std::is_same_v<decltype(a_value), const_value_t>) {
      for (const auto& kv_pair : a_this.m_storage)
        if (kv_pair.second == a_value)
          return kv_pair.first;
    }

    contract_assert(false);
  }
}