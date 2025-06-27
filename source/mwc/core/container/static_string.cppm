module;

export module mwc_static_string;

import mwc_definition;

import std;

export namespace mwc {
  // wrapper around a static array of characters
  template <size_t tp_count>
  struct static_string_st {
    constexpr static_string_st(const char_t (&a_string)[tp_count + 1]) : m_data {} {
      std::copy_n(a_string, tp_count + 1, std::data(m_data));
    }
    constexpr static_string_st(const string_view_t a_string_view) : m_data {} {
      std::copy_n(a_string_view.data(), tp_count + 1, std::data(m_data));
    }
    [[nodiscard]] constexpr operator string_view_t() const {
      return {std::data(m_data), tp_count};
    }

    array_t<char_t, tp_count + 1> m_data;
  };

  // explicit deduction guides
  template <size_t tp_count>
  static_string_st(const char_t (&a_string)[tp_count]) -> static_string_st<tp_count - 1>;
  static_string_st(const auto... a_chars) -> static_string_st<sizeof...(a_chars)>;
}