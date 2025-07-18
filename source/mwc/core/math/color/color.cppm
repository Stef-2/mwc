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

    using math_t::math_t;

    auto red() const -> const tp;
    auto green() const -> const tp;
    auto blue() const -> const tp;
    auto alpha() const -> const tp;
  };

  // implementation
  template <std::floating_point tp>
  auto color_st<tp>::red() const -> const tp {
    return math_t::x();
  }
  template <std::floating_point tp>
  auto color_st<tp>::green() const -> const tp {
    return math_t::y();
  }
  template <std::floating_point tp>
  auto color_st<tp>::blue() const -> const tp {
    return math_t::z();
  }
  template <std::floating_point tp>
  auto color_st<tp>::alpha() const -> const tp {
    return math_t::w();
  }
}