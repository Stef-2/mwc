module;

export module mwc_ct_counter;

import mwc_incomplete_type;

import std;

export namespace mwc {
  namespace meta {
    template <typename tp>
    struct counter_st {
      static consteval auto value() -> size_t {
        auto constant = size_t {0};
        while (std::meta::is_complete_type(std::meta::substitute(^^incomplete_st,
                                                                 {
                                                                 std::meta::reflect_constant(constant)})))
          ++constant;
        return constant;
      }
      static consteval auto increment() -> void {
        std::meta::define_aggregate(std::meta::substitute(^^incomplete_st,
                                                          {
                                                          std::meta::reflect_constant(value())}),
                                    {});
      }
    };
  }
}