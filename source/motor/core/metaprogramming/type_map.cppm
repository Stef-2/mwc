module;

export module mtr_type_map;

import mtr_concept;

import std;

export namespace mtr
{
  template <typename key, typename value>
  struct type_map_st
  {
    using key_t = key;
    using value_t = value;
  };

  template <auto EnumValue>
  struct enum_type
  {
    using type = std::decay_t<decltype(EnumValue)>;
  };

  template <auto EnumValue>
  using enum_type_t = typename enum_type<EnumValue>::type;

  template <auto EnumValue, typename Type>
  // requires concepts::enumerator<enumerator>
  struct enum_type_map_st
  {
    using enum_t = enum_type_t<EnumValue>;
    using type_t = Type;
    // private:
    //  using enum_value_t = decltype(enumerator);

    /*template <enumerator>
    struct enum_st
    {
      using type_t = type;
    };*/

    // static constexpr enum_st<enumerator> x = {};

    /*public:
    using type_t = decltype(x)::type_t;*/
  };

  enum wtf
  {
    x,
    y,
    z
  };

  using testt = enum_type_t<wtf::x>;

  // testt t = 5;

  using omg = decltype(wtf::x);
  // omg o = 5;

  using mapped_type = enum_type_map_st<wtf::x, float>::type_t;
  using mapped_enum = enum_type_map_st<wtf::x, float>::enum_t;
}

// using bind = mtr::enum_type_map_st<wtf::x, float>::type_t;
//  mtr::enum_type_map_st<wtf, wtf::x, int>::enum_st