module;

#include "mwc/core/contract/natural_syntax.hpp"

export module mwc_data;

import mwc_definition;
import mwc_observer_ptr;
import mwc_contract_assertion;
import mwc_concept;

import std;

export namespace mwc {
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
      pre(contract::validate_data_size(a_data));

    [[nodiscard]] constexpr auto data(this auto&& a_this) post(r : r != nullptr);
    [[nodiscard]] constexpr auto size() const post(r : r != 0);
    template <typename tp_this>
    [[nodiscard]] constexpr auto span(this tp_this&& a_this)
      post(r : contract::validate_data_size(r));
    constexpr auto begin(this auto&& a_this);
    constexpr auto end(this auto&& a_this);

    static constexpr size_t s_extent = tp_size;
    observer_ptr_t<tp> m_data;
    size_t m_size;
  };

  // explicit deduction guide
  template <typename tp>
    requires concepts::contiguous_storage_c<tp>
  data_st(const auto& a_data) -> data_st<typename decltype(a_data)::value_type>;

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
  constexpr auto data_st<tp, tp_size>::span(this auto&& a_this) {
    return span_t<tp, tp_size> {a_this.m_data, a_this.m_size};
  }
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::end(this auto&& a_this) {
    return a_this.m_data + a_this.m_size;
  }
  template <typename tp, size_t tp_size>
  constexpr auto data_st<tp, tp_size>::begin(this auto&& a_this) {
    return a_this.m_data;
  }
}