module;

export module mwc_minimal_integral;

import mwc_definition;

import std;

export namespace mwc
{
  namespace utility
  {
    template <size_t tp_value>
    struct minimal_integral_st
    {
      private:
      using width8_t =
        std::conditional_t<tp_value <= std::numeric_limits<uint8_t>::max(),
                           std::true_type, std::false_type>;
      using width16_t =
        std::conditional_t<tp_value <= std::numeric_limits<uint16_t>::max(),
                           std::true_type, std::false_type>;
      using width32_t =
        std::conditional_t<tp_value <= std::numeric_limits<uint32_t>::max(),
                           std::true_type, std::false_type>;
      using width64_t =
        std::conditional_t<tp_value <= std::numeric_limits<uint64_t>::max(),
                           std::true_type, std::false_type>;

      public:
      using minimal_t = std::conditional_t<
        width8_t::value, uint8_t,
        std::conditional_t<
          width16_t::value, uint16_t,
          std::conditional_t<width32_t::value, uint32_t, uint64_t>>>;
    };
  }
}