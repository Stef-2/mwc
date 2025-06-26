module;

export module mwc_optional;

import mwc_definition;

export namespace mwc {
  // structural alternative to std::optional
  // suitable for use as a template parameter
  template <typename tp>
  struct optional_st {
    constexpr optional_st();
    constexpr optional_st(const tp& a_data);

    [[nodiscard]] constexpr operator bool_t(this auto&& a_this);

    tp m_data;
    bool_t m_void;
  };

  // implementation
  template <typename tp>
  constexpr optional_st<tp>::optional_st() : m_data {},
                                             m_void {true} {}
  template <typename tp>
  constexpr optional_st<tp>::optional_st(const tp& a_data) : m_data {a_data},
                                                             m_void {false} {}
  template <typename tp>
  constexpr optional_st<tp>::operator bool_t(this auto&& a_this) {
    return not a_this.m_void;
  }
}