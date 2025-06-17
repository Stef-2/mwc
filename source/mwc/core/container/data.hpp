#pragma once

import mwc_definition;
import mwc_observer_ptr;
import mwc_contract_assertion;
import mwc_concept;

import std;

namespace mwc {
  // type erased non-owning structural span over contiguous storage
  struct void_data_st {
    observer_ptr_t<void> m_data;
    size_t m_size;
  };

  // typed non-owning structural span over contiguous storage
  template <typename tp, size_t tp_size = std::dynamic_extent>
  struct data_st {
    constexpr data_st();
    constexpr data_st(observer_ptr_t<tp> a_data, const size_t a_size)
      pre(a_data != nullptr and a_size != 0);
    constexpr data_st(const concepts::contiguous_storage_c auto& a_data)
      pre(contract::validate_storage(a_data));

    [[nodiscard]] constexpr auto data(this auto&& a_this) post(r : r != nullptr);
    [[nodiscard]] constexpr auto size() const post(r : r != 0);
    [[nodiscard]] constexpr auto size_bytes() const post(r : r != 0);
    [[nodiscard]] constexpr auto empty() const;
    template <typename tp_this>
    [[nodiscard]] constexpr auto span(this tp_this&& a_this)
      post(r : contract::validate_storage(r));

    constexpr auto begin() const pre(m_size != 0) post(r : r != nullptr);
    constexpr auto end() const pre(m_size != 0)
      post(r : r != nullptr and r != begin());

    static constexpr size_t extent = tp_size;
    observer_ptr_t<tp> m_data;
    size_t m_size;
  };

  // explicit deduction guide
  // clang doesn't like this
  /*
  template <typename tp, size_t tp_size>
    requires concepts::contiguous_storage_c<tp>
  data_st(const auto& a_data) -> data_st<typename decltype(a_data)::value_type, tp_size>;
  */

  // implementation
  template <typename tp, size_t tp_size>
  constexpr data_st<tp, tp_size>::data_st() : m_data {nullptr},
                                              m_size {0} {}
  template <typename tp, size_t tp_size>
  constexpr data_st<tp, tp_size>::data_st(observer_ptr_t<tp> a_data,
                                          const size_t a_size)
  : m_data {a_data},
    m_size {a_size} {}
  template <typename tp, size_t tp_size>
  constexpr data_st<tp, tp_size>::data_st(
    const concepts::contiguous_storage_c auto& a_data)
  : m_data {(tp*)a_data.data()},
    m_size {a_data.size()} {}
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::data(this auto&& a_this) {
    return a_this.m_data;
  }
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::size() const {
    return m_size;
  }
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::size_bytes() const {
    return m_size * sizeof(tp);
  }
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::empty() const {
    return m_size == 0;
  }
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::span(this auto&& a_this) {
    return span_t<tp, tp_size> {a_this.m_data, a_this.m_size};
  }
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::begin() const/* pre(m_size != 0)
    post(r : r != nullptr)*/ {
    return m_data;
  }
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::end() const {
    return m_data + m_size;
  }
}