module;

export module mtr_type_map;

export namespace mtr
{
  template <typename key, typename value>
  struct type_map_st
  {
    using key_t = key;
    using value_t = value;
  };

  template <typename enumerator, enumerator enum_value, typename type>
  struct enum_type_map_st
  {
    private:
    template <enumerator e = enum_value>
    struct enum_st
    {
      using type_t = type;
    };

    static constexpr enum_st x = {};

    public:
    using type_t = decltype(x)::type_t;
  };
}

enum wtf
{
  x,
  y,
  z
};

using omg = decltype(wtf::x);
// mtr::enum_type_map_st<wtf, wtf::x, int>::enum_st