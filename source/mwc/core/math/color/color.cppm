module;

export module mwc_color;

import mwc_definition;
import mwc_math_definition;

import std;

export namespace mwc {
  template <std::floating_point tp = float32_t>
  struct color_st : public math::vector_t<tp, 4> {
    public:
    using math_t = math::vector_t<tp, 4>;
    using integral_value_t = uint8_t;

    static constexpr auto integral_value_max = std::numeric_limits<uint8_t>::max();

    constexpr color_st() = default;
    constexpr color_st(const tp a_red, const tp a_green, const tp a_blue, const tp a_alpha = 1.0);
    constexpr color_st(const integral_value_t a_red, const integral_value_t a_green, const integral_value_t a_blue,
                       const integral_value_t a_alpha = integral_value_max);

    template <typename tp_this>
    [[nodiscard]] auto red(this tp_this&& a_this) -> decltype(auto);
    template <typename tp_this>
    [[nodiscard]] auto green(this tp_this&& a_this) -> decltype(auto);
    template <typename tp_this>
    [[nodiscard]] auto blue(this tp_this&& a_this) -> decltype(auto);
    template <typename tp_this>
    [[nodiscard]] auto alpha(this tp_this&& a_this) -> decltype(auto);
  };

  // implementation
  template <std::floating_point tp>
  constexpr color_st<tp>::color_st(const tp a_red, const tp a_green, const tp a_blue, const tp a_alpha)
  : math_t {a_red, a_green, a_blue, a_alpha} {}
  template <std::floating_point tp>
  constexpr color_st<tp>::color_st(const integral_value_t a_red, const integral_value_t a_green, const integral_value_t a_blue,
                                   const integral_value_t a_alpha)
  : math_t {static_cast<tp>(a_red) / integral_value_max, static_cast<tp>(a_green) / integral_value_max,
            static_cast<tp>(a_blue) / integral_value_max, static_cast<tp>(a_alpha) / integral_value_max} {}
  template <std::floating_point tp>
  template <typename tp_this>
  auto color_st<tp>::red(this tp_this&& a_this) -> decltype(auto) {
    return std::forward_like<decltype(a_this)>(a_this.x());
  }
  template <std::floating_point tp>
  template <typename tp_this>
  auto color_st<tp>::green(this tp_this&& a_this) -> decltype(auto) {
    return std::forward_like<decltype(a_this)>(a_this.y());
  }
  template <std::floating_point tp>
  template <typename tp_this>
  auto color_st<tp>::blue(this tp_this&& a_this) -> decltype(auto) {
    return std::forward_like<decltype(a_this)>(a_this.z());
  }
  template <std::floating_point tp>
  template <typename tp_this>
  auto color_st<tp>::alpha(this tp_this&& a_this) -> decltype(auto) {
    return std::forward_like<decltype(a_this)>(a_this.w());
  }
}