module;

#include "mwc/core/contract/natural_syntax.hpp"

export module mwc_data;

import mwc_definition;
import mwc_observer_ptr;
import mwc_contract_assertion;
import mwc_concept;

import std;

export namespace mwc {
  // structural span over contiguous storage
  template <typename tp>
  struct data_st {
    constexpr data_st();
    constexpr data_st(observer_ptr_t<tp> a_data, const size_t a_size)
      pre(a_data != nullptr and a_size != 0);
    constexpr data_st(concepts::contiguous_storage_c auto& a_data)
      pre(contract::validate_data_size(a_data));
    [[nodiscard]] constexpr auto data(this auto&& a_this) post(r : r != nullptr);
    [[nodiscard]] constexpr auto size() const post(r : r != 0);
    template <typename tp_this>
    [[nodiscard]] constexpr auto span(this tp_this&& a_this) {
      if consteval {
        return span_t<tp, std::declval<tp_this>().size()> {a_this.m_data};
      } /*else {
        return span_t<tp> {tp_this.m_data, tp_this.m_size};
      }*/
    }
    post(r : contract::validate_data_size(r));
    constexpr auto begin(this auto&& a_this);
    constexpr auto end(this auto&& a_this);

    observer_ptr_t<tp> m_data;
    size_t m_size;
  };

  // explicit deduction guide
  data_st(concepts::contiguous_storage_c auto a_data)
    -> data_st<typename decltype(a_data)::value_type>;

  // implementation
  template <typename tp>
  constexpr data_st<tp>::data_st() : m_data {nullptr},
                                     m_size {0} {}
  template <typename tp>
  constexpr data_st<tp>::data_st(observer_ptr_t<tp> a_data, const size_t a_size)
  : m_data {a_data},
    m_size {a_size} {}
  template <typename tp>
  constexpr data_st<tp>::data_st(concepts::contiguous_storage_c auto& a_data)
  : m_data {a_data.data()},
    m_size {a_data.size()} {}
  template <typename tp>
  constexpr auto data_st<tp>::data(this auto&& a_this) {
    return a_this.m_data;
  }
  template <typename tp>
  constexpr auto data_st<tp>::size() const {
    return m_size;
  }
  /*template <typename tp>
  constexpr auto data_st<tp>::span(this auto&& a_this) {
    if consteval {
      return span_t<tp, a_this.m_size> {a_this.m_data};
    } else {
      return span_t<tp> {a_this.m_data, a_this.m_size};
    }
  }*/
  template <typename tp>
  constexpr auto data_st<tp>::end(this auto&& a_this) {
    return a_this.m_data + a_this.m_size;
  }
  template <typename tp>
  constexpr auto data_st<tp>::begin(this auto&& a_this) {
    return a_this.m_data;
  }
}