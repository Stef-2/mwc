module;

export module mtr_type_map;

import mtr_concept;

import std;

export namespace mtr
{
  // map [key_type] to [value_type]
  template <typename key_type, typename value_type>
  struct type_map_st
  {
    using key_t = key_type;
    using value_t = value_type;
  };

  // map [enum_key_value] to [value_type]
  // enum_key_value must be a constant value expression
  template <auto enum_key_value, typename value_type>
  struct enum_map_st
  {
    using enum_t = decltype(enum_key_value);

    static constexpr inline auto s_enum_value = enum_key_value;
    using value_t = value_type;
  };
}