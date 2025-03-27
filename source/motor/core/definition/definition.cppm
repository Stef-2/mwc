module;

export module mtr_definition;

import std;

export namespace mtr
{
  // integral types
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

  // floating point types
  using float16_t = std::float16_t;
  using bfloat16_t = std::bfloat16_t;
  using float32_t = std::float32_t;
  using float64_t = std::float64_t;
  using float128_t = std::float128_t;

  // character / string types
  using char_t = char;
  using string_t = std::basic_string<char_t>;
  using string_view_t = std::basic_string_view<char_t>;

  // path types
  using filepath_t = std::filesystem::path;
}