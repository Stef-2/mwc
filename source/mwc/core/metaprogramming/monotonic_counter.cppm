module;

export module mwc_monotonic_counter;

import mwc_definition;
import mwc_incomplete_type;

import std;

export namespace mwc {
  namespace meta {
    // steady, monotonic compile time counter
    // works by counting complete specializations of [incomplete_type_v_st]
    // [tp_generation] parameter allows for existence of independent counters
    // i.e. monotonic_counter_st<0> is independent of monotonic_counter_st<1>
    template <size_t tp_generation = size_t {0}>
    struct monotonic_counter_st {
      static consteval auto value() -> size_t {
        auto constant = size_t {0};
        while (std::meta::is_complete_type(
          std::meta::substitute(^^incomplete_type_v_st,
                                {
                                std::meta::reflect_constant(constant), std::meta::reflect_constant(tp_generation)})))
          ++constant;

        return constant;
      }
      static consteval auto increment() -> void {
        std::meta::define_aggregate(
          std::meta::substitute(^^incomplete_type_v_st,
                                {
                                std::meta::reflect_constant(value()), std::meta::reflect_constant(tp_generation)}),
          {});
      }
    };
  }
}