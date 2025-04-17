module;

export module mwc_static_map;

import mwc_definition;
import mwc_assert;
import mwc_concept;
import mwc_minimal_integral;

import std;

export namespace mwc {
  template <typename tp_key, typename tp_value, size_t tp_size>
  struct static_bimap_st {
    using key_t = tp_key;
    using const_key_t = std::add_const_t<key_t>;
    using value_t = tp_value;
    using const_value_t = std::add_const_t<value_t>;
    using storage_t = std::array<pair_t<key_t, value_t>, tp_size>;

    template <typename tp>
    [[nodiscard]] constexpr auto operator[](this tp&& a_this,
                                            const auto a_value) -> auto&
      requires concepts::any_of_c<decltype(a_value), const_key_t, const_value_t>
    {
      if constexpr (std::is_same_v<decltype(a_value), const_key_t>) {
        for (const auto& kv_pair : a_this.m_storage)
          if (kv_pair.first == a_value)
            return kv_pair.second;
      }

      if constexpr (std::is_same_v<decltype(a_value), const_value_t>) {
        for (const auto& kv_pair : a_this.m_storage)
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
  static_bimap_st(const pair_t<tp_key, tp_value> (&a)[tp_size])
    -> static_bimap_st<tp_key, tp_value, tp_size>;

  template <typename tp_key,
            size_t tp_key_count,
            typename tp_value,
            size_t tp_value_count>
    requires(not std::is_same_v<tp_key, tp_value> and
             tp_key_count <= tp_value_count)
  struct static_multimap_st {
    using key_t = tp_key;
    using const_key_t = std::add_const_t<key_t>;
    using value_t = tp_value;
    using const_value_t = std::add_const_t<value_t>;
    using kv_pair_t = pair_t<key_t, value_t>;
    using value_index_t =
      utility::minimal_integral_st<tp_value_count + 1>::type;

    struct key_st {
      tp_key m_key;
      value_index_t m_begin;
      value_index_t m_end;
    };

    constexpr static_multimap_st(
      const kv_pair_t (&a_kv_pairs)[std::max(tp_key_count, tp_value_count)])
    : m_keys {},
      m_values {} {
      auto input_array = std::to_array(a_kv_pairs);
      std::ranges::sort(input_array,
                        [](const kv_pair_t a_current, const kv_pair_t a_next) {
                          return a_current.first < a_next.first;
                        });

      for (value_index_t i = {}, key_count = {}; i < tp_value_count; ++i) {
        const auto current = input_array[i];
        const auto key_exists = [this, &current]() -> bool {
          for (const auto& key : m_keys)
            if (key.m_key == current.first)
              return true;
          return false;
        }();

        m_values[i] = current.second;

        if (not key_exists) {
          m_keys[key_count] = {current.first, i,
                               static_cast<value_index_t>(i + 1)};
          ++key_count;
        } else {
          m_keys[key_count - 1].m_end = i + 1;
        }
      }
    }

    template <typename tp>
    [[nodiscard]] constexpr auto operator[](this tp&& a_this,
                                            const auto a_value) -> const auto
      requires concepts::any_of_c<decltype(a_value), const_key_t, const_value_t>
    {
      if constexpr (std::is_same_v<decltype(a_value), const_key_t>)
        for (const auto& key : a_this.m_keys)
          if (key.m_key == a_value)
            return span_t {a_this.m_values.begin() + 1 /*key.m_begin*/,
                           a_this.m_values.begin() + 2 /*key.m_end*/};

      if constexpr (std::is_same_v<decltype(a_value), const_value_t>)
        for (const auto& key : a_this.m_keys)
          for (auto i = key.m_begin; i < key.m_end; ++i)
            if (a_this.m_values[i] == a_value)
              return key.m_key;

      diagnostic::assert(false, "requested value not found");
      std::unreachable();
    }

    array_t<key_st, tp_key_count> m_keys;
    array_t<tp_value, tp_value_count> m_values;
  };

  void test() {
    // fuck off
    constexpr auto map = static_bimap_st {{pair_t {1, 2.0f}, {3, 4.0f}}};
    constexpr auto f = map[1];
    constexpr auto i = map[2.0f];
    auto wtf = f + i;
  }
}