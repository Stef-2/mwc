#pragma once

#include "mwc/core/contract/assertion.hpp"
#include "mwc/core/definition/definition.hpp"


#include <type_traits>
#include <algorithm>

//import mwc_definition;
import mwc_concept;
import mwc_minimal_integral;

namespace mwc {
  // static storage bidirectional multimap
  // suitable for use in constant evaluation contexts
  template <typename tp_key, size_t tp_key_count, typename tp_value, size_t tp_value_count>
    requires(not std::is_same_v<tp_key, tp_value> and
             tp_key_count <= tp_value_count)
  struct static_bi_multimap_st {
    using key_t = tp_key;
    using const_key_t = std::add_const_t<key_t>;
    using value_t = tp_value;
    using const_value_t = std::add_const_t<value_t>;
    using kv_pair_t = pair_t<key_t, value_t>;
    using value_index_t =
      typename utility::minimal_integral_st<tp_value_count + 1>::type;

    struct key_st {
      tp_key m_key;
      value_index_t m_begin;
      value_index_t m_end;
    };

    constexpr static_bi_multimap_st(const span_t<const kv_pair_t, tp_value_count>
                                      a_kv_pairs);

    template <typename tp_this>
    [[nodiscard]] constexpr auto
    operator[](this tp_this&& a_this, const auto a_value)
      /*pre(contract::validate_data_size(a_this.m_keys))
        pre(contract::validate_data_size(a_this.m_values))*/
      -> decltype(auto)
      requires concepts::any_of_c<decltype(a_value), const_key_t, const_value_t>;
    //template <typename tp_this>
    [[nodiscard]] constexpr auto
    equal_range(/*this tp_this&& a_this, */const key_t a_key) -> decltype(auto)
      pre(m_keys.size() != 0)
        /*pre(contract::validate_data_size(a_this.m_values))*/;

    constexpr auto begin() const pre(not m_keys.empty()) post(r : r != nullptr);
    constexpr auto end() const pre(not m_keys.empty())
      post(r : r != nullptr and r != begin());

    array_t<key_st, tp_key_count> m_keys;
    array_t<tp_value, tp_value_count> m_values;
  };

  // implementation
  template <typename tp_key, size_t tp_key_count, typename tp_value, size_t tp_value_count>
    requires(not std::is_same_v<tp_key, tp_value> &&
             tp_key_count <= tp_value_count)
  constexpr static_bi_multimap_st<tp_key, tp_key_count, tp_value, tp_value_count>::
    static_bi_multimap_st(const span_t<const kv_pair_t, tp_value_count> a_kv_pairs)
  : m_keys {},
    m_values {} {
    array_t<kv_pair_t, tp_value_count> input_array;
    std::ranges::copy(a_kv_pairs, input_array.begin());

    // sort the input by keys
    std::ranges::sort(input_array,
                      [](const kv_pair_t a_current, const kv_pair_t a_next) {
                        return a_current.first < a_next.first;
                      });

    // generate key - value pairs
    for (value_index_t i = {}, key_count = {}; i < tp_value_count; ++i) {
      const auto& current = input_array[i];

      const auto key_exists = [this, &current]() -> bool {
        for (const auto& key : m_keys)
          if (key.m_key == current.first)
            return true;
        return false;
      }();

      m_values[i] = current.second;

      if (not key_exists) {
        m_keys[key_count] = {current.first, i, static_cast<value_index_t>(i + 1)};
        ++key_count;
      } else {
        m_keys[key_count - 1].m_end = i + 1;
      }
    }
  }

  template <typename tp_key, size_t tp_key_count, typename tp_value, size_t tp_value_count>
    requires(not std::is_same_v<tp_key, tp_value> &&
             tp_key_count <= tp_value_count)
  constexpr auto
  static_bi_multimap_st<tp_key, tp_key_count, tp_value, tp_value_count>::operator[](
    this auto&& a_this,
    const auto a_value) -> decltype(auto)
    requires concepts::any_of_c<decltype(a_value),
                                std::add_const_t<tp_key>,
                                std::add_const_t<tp_value>>
  {
    if constexpr (std::is_same_v<decltype(a_value), const_key_t>)
      for (const auto& key : a_this.m_keys)
        if (key.m_key == a_value)
          return pair_t {a_this.m_values.begin() + key.m_begin,
                         a_this.m_values.begin() + key.m_end};

    if constexpr (std::is_same_v<decltype(a_value), const_value_t>)
      for (const auto& key : a_this.m_keys)
        for (auto i = key.m_begin; i < key.m_end; ++i)
          if (a_this.m_values[i] == a_value)
            return key.m_key;

    contract_assert(false);
  }
  template <typename tp_key, size_t tp_key_count, typename tp_value, size_t tp_value_count>
    requires(not std::is_same_v<tp_key, tp_value> && tp_key_count <= tp_value_count)
  constexpr auto
  static_bi_multimap_st<tp_key, tp_key_count, tp_value, tp_value_count>::equal_range(
    /*this auto&& a_this,*/
    const key_t a_key) -> decltype(auto) {
    return this->operator[](a_key);
  }
  template <typename tp_key, size_t tp_key_count, typename tp_value, size_t tp_value_count>
    requires(not std::is_same_v<tp_key, tp_value> && tp_key_count <= tp_value_count)
  constexpr auto
  static_bi_multimap_st<tp_key, tp_key_count, tp_value, tp_value_count>::begin() const {
    return m_keys.begin();
  }
  template <typename tp_key, size_t tp_key_count, typename tp_value, size_t tp_value_count>
    requires(not std::is_same_v<tp_key, tp_value> && tp_key_count <= tp_value_count)
  constexpr auto
  static_bi_multimap_st<tp_key, tp_key_count, tp_value, tp_value_count>::end() const {
    return m_keys.end();
  }
}