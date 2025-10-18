module;

export module mwc_definition;

import std;

export namespace mwc {
  // integral numeric types
  using sint8_t = std::int8_t;
  using uint8_t = std::uint8_t;
  using sint16_t = std::int16_t;
  using uint16_t = std::uint16_t;
  using sint32_t = std::int32_t;
  using uint32_t = std::uint32_t;
  using sint64_t = std::int64_t;
  using uint64_t = std::uint64_t;
  using sintptr_t = std::intptr_t;
  using uintptr_t = std::uintptr_t;
  using size_t = std::size_t;

// floating point numeric types
// note: change these once clang implements <stdfloat>
#ifdef __clang__
  using float16_t = __fp16;
  using bfloat16_t = __bf16;
  using float32_t = float;
  using float64_t = double;
  using float128_t = long double;
#else
  using float16_t = std::float16_t;
  using bfloat16_t = std::bfloat16_t;
  using float32_t = std::float32_t;
  using float64_t = std::float64_t;
  using float128_t = std::float128_t;
#endif

  // byte / character / string types
  using byte_t = std::byte;
  using char_t = char;
  using string_t = std::basic_string<char_t>;
  using string_view_t = std::basic_string_view<char_t>;

  // utility types
  using bool_t = bool;
  using nullptr_t = std::nullptr_t;
  using any_t = std::any;

  // path types
  using file_path_t = std::filesystem::path;

  // stream types
  using istream_t = std::istream;
  using ostream_t = std::ostream;
  using iostream_t = std::iostream;
  using file_t = std::FILE;

  // chrono types
  using high_resolution_clock_t = std::chrono::high_resolution_clock;
  using system_clock_t = std::chrono::system_clock;
  using time_point_t = std::chrono::time_point<high_resolution_clock_t>;
  using clock_precision_t = std::nano;
  using integral_duration_t = std::chrono::duration<uint64_t, clock_precision_t>;
  using floating_duration_t = std::chrono::duration<float64_t, clock_precision_t>;
  using default_duration_t = std::chrono::nanoseconds;

  // container types
  template <typename tp, size_t tp_count>
  using array_t = std::array<tp, tp_count>;
  template <typename tp, typename tp_allocator = std::allocator<tp>>
  using vector_t = std::vector<tp, tp_allocator>;
  template <typename tp>
  using initializer_list_t = std::initializer_list<tp>;
  template <typename tp, size_t tp_size = std::dynamic_extent>
  using span_t = std::span<tp, tp_size>;
  template <typename... tp>
  using tuple_t = std::tuple<tp...>;
  template <typename tp>
  using optional_t = std::optional<tp>;
  template <typename tp_first, typename tp_second>
  using pair_t = std::pair<tp_first, tp_second>;

  template <typename tp_key, typename tp_value, typename tp_compare = std::less<tp_key>,
            typename tp_allocator = std::allocator<pair_t<const tp_key, tp_value>>>
  using map_t = std::map<tp_key, tp_value, tp_compare, tp_allocator>;
  template <typename tp_key, typename tp_value, typename tp_hash = std::hash<tp_key>,
            typename tp_key_equal = std::equal_to<tp_key>, typename tp_allocator = std::allocator<pair_t<const tp_key, tp_value>>>
  using unordered_map_t = std::unordered_map<tp_key, tp_value, tp_hash, tp_key_equal, tp_allocator>;
  template <typename tp_key, typename tp_value, typename tp_compare = std::less<tp_key>,
            typename tp_allocator = std::allocator<pair_t<const tp_key, tp_value>>>
  using multimap_t = std::multimap<tp_key, tp_value, tp_compare, tp_allocator>;
  template <typename tp_key, typename tp_value, typename tp_hash = std::hash<tp_key>,
            typename tp_key_equal = std::equal_to<tp_key>, typename tp_allocator = std::allocator<pair_t<const tp_key, tp_value>>>
  using unordered_multimap_t = std::unordered_multimap<tp_key, tp_value, tp_hash, tp_key_equal, tp_allocator>;

  template <typename tp_key, typename tp_compare = std::less<tp_key>, typename tp_allocator = std::allocator<tp_key>>
  using set_t = std::set<tp_key, tp_compare, tp_allocator>;
  template <typename tp_key, typename tp_hash = std::hash<tp_key>, typename tp_key_equal = std::equal_to<tp_key>,
            typename tp_allocator = std::allocator<tp_key>>
  using unordered_set_t = std::unordered_set<tp_key, tp_hash, tp_key_equal, tp_allocator>;
  template <typename tp_key, typename tp_compare = std::less<tp_key>, typename tp_allocator = std::allocator<tp_key>>
  using multiset_t = std::multiset<tp_key, tp_compare, tp_allocator>;
  template <typename tp_key, typename tp_hash = std::hash<tp_key>, typename tp_key_equal = std::equal_to<tp_key>,
            typename tp_allocator = std::allocator<tp_key>>
  using unordered_multiset_t = std::unordered_multiset<tp_key, tp_hash, tp_key_equal, tp_allocator>;
}