module;

export module mwc_static_map;

import mwc_definition;
import mwc_assert;
import mwc_concept;
import mwc_minimal_integral;

import std;

export namespace mwc
{
  template <typename tp_key, typename tp_value, size_t tp_size>
  struct static_map_st
  {
    using key_t = tp_key;
    using const_key_t = std::add_const_t<key_t>;
    using value_t = tp_value;
    using const_value_t = std::add_const_t<value_t>;
    using storage_t = std::array<pair_t<key_t, value_t>, tp_size>;

    template <typename tp>
    [[nodiscard]] constexpr auto operator[](this tp&& m_this,
                                            const auto a_value) -> auto&
      requires concepts::any_of_c<decltype(a_value), const_key_t, const_value_t>
    {
      for (const auto& kv_pair : m_this.m_storage)
      {
        if constexpr (std::is_same_v<decltype(a_value), const_key_t>)
          if (kv_pair.first == a_value)
            return kv_pair.second;

        if constexpr (std::is_same_v<decltype(a_value), const_value_t>)
          if (kv_pair.second == a_value)
            return kv_pair.first;
      }

      diagnostic::assert(false, "requested value not found");
      std::unreachable();
    }

    storage_t m_storage;
  };

  // explicit deduction guide
  template <typename tp_key, typename tp_value, size_t tp_size>
  static_map_st(const pair_t<tp_key, tp_value> (&a)[tp_size])
    -> static_map_st<tp_key, tp_value, tp_size>;

  template <typename tp_key,
            size_t tp_key_count,
            typename tp_value,
            size_t tp_value_count>
    requires(not std::is_same_v<tp_key, tp_value> and
             tp_key_count <= tp_value_count)
  struct static_multimap_st
  {
    using key_t = tp_key;
    using const_key_t = std::add_const_t<key_t>;
    using value_t = tp_value;
    using const_value_t = std::add_const_t<value_t>;
    using kv_pair_t = pair_t<key_t, value_t>;
    using value_index_t = utility::minimal_integral_st<tp_value_count>::type;

    struct key_st
    {
      tp_key m_key;
      value_index_t m_begin;
      value_index_t m_end;
    };

    constexpr static_multimap_st(
      const kv_pair_t (&a_keys)[std::max(tp_key_count, tp_value_count)])
    : m_keys {},
      m_values {}
    {
      auto input_array = std::to_array(a_keys);

      std::ranges::sort(input_array,
                        [](const kv_pair_t a_current, const kv_pair_t a_next)
                        { return a_current.first < a_next.first; });

      for (auto i = value_index_t {0}, key_count {0}; i < tp_value_count; ++i)
      {
        //const auto key_exists = std::ranges::binary_search(m_keys, key);

        if (not false)
          const auto value_begin_index = i;
      }
    }

    array_t<key_st, tp_key_count> m_keys;
    array_t<tp_value, tp_value_count> m_values;
  };

  void test()
  {
    constexpr auto map =
      static_map_st<int, float, 2> {pair_t {1, 2.0f}, {3, 4.0f}};
    constexpr auto f = map[1];
    constexpr auto i = map[4.0f];
    constexpr auto map2 = static_map_st {{pair_t {1, 2.0f}, {3, 4.0f}}};

    constexpr auto map3 = static_multimap_st<int, 2, float, 2> {
      {pair_t {1, 2.0f}, pair_t {3, 4.0f}}};
  }
}