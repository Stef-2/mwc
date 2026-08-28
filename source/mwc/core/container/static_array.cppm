module;

export module mwc_static_array;

import mwc_definition;
import mwc_contract_assertion;

import std;

export namespace mwc {
  template <typename tp, size_t tp_count>
    requires requires { tp_count > 0 and tp_count < std::numeric_limits<size_t>::max(); }
  struct static_array_st {
    static_assert(tp_count > 0 and tp_count < std::numeric_limits<size_t>::max(), "invalid [tp_count] value");

    using storage_t = array_t<tp, tp_count>;

    constexpr static_array_st(const tp (&a_data)[tp_count]) pre(tp_count > 0 and tp_count < std::numeric_limits<size_t>::max());
    constexpr static_array_st(const span_t<const tp, tp_count> a_data) pre(contract::validate_storage(a_data));

    template <typename tp_this>
    [[nodiscard]] constexpr auto array(this tp_this&& a_this) -> decltype(auto);
    [[nodiscard]] constexpr auto size() const -> size_t;

    storage_t m_data;
  };

  // explicit deduction guides
  template <typename tp, size_t tp_count>
    requires requires { tp_count > 0 and tp_count < std::numeric_limits<size_t>::max(); }
  static_array_st(const tp (&a_data)[tp_count]) -> static_array_st<tp, tp_count>;
  static_array_st(const auto... a_data) -> static_array_st<decltype(a_data...[0]), sizeof...(a_data)>;

  // implementation
  template <typename tp, size_t tp_count>
    requires requires { tp_count > 0 && tp_count < std::numeric_limits<size_t>::max(); }
  constexpr static_array_st<tp, tp_count>::static_array_st(const tp (&a_data)[tp_count]) : m_data {std::to_array(a_data)} {}
  template <typename tp, size_t tp_count>
    requires requires { tp_count > 0 && tp_count < std::numeric_limits<size_t>::max(); }
  constexpr static_array_st<tp, tp_count>::static_array_st(const span_t<const tp, tp_count> a_data) : m_data {} {
    std::copy_n(a_data.data(), tp_count, m_data.data());
  }
  template <typename tp, size_t tp_count>
    requires requires { tp_count > 0 && tp_count < std::numeric_limits<size_t>::max(); }
  template <typename tp_this>
  constexpr auto ::mwc::static_array_st<tp, tp_count>::array(this tp_this&& a_this) -> decltype(auto) {
    return std::forward_like<tp_this>(a_this.m_data);
  }
  template <typename tp, size_t tp_count>
    requires requires { tp_count > 0 && tp_count < std::numeric_limits<size_t>::max(); }
  constexpr auto static_array_st<tp, tp_count>::size() const -> size_t {
    return m_data.size();
  }
}