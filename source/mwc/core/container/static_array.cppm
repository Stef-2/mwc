module;

export module mwc_static_array;

import mwc_definition;

import std;

export namespace mwc {
  template <typename tp, size_t tp_count>
  struct static_array_st {
    using storage_t = array_t<tp, tp_count>;

    constexpr static_array_st(const tp (&a_data)[tp_count]) : m_data {std::to_array(a_data)} {}

    constexpr static_array_st(const span_t<const tp, tp_count> a_data) : m_data {} {
      std::copy_n(a_data.data(), tp_count, m_data.data());
    }

    storage_t m_data;
  };

  // explicit deduction guides
  template <typename tp, size_t tp_count>
  static_array_st(const tp (&a_data)[tp_count]) -> static_array_st<tp, tp_count>;
  static_array_st(const auto... a_data) -> static_array_st<decltype(a_data...[0]), sizeof...(a_data)>;
}