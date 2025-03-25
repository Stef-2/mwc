module;

export module mtr_definition;

import std;

export namespace mtr
{
    // integral types
    using s8 = std::int8_t;
    using u8 = std::uint8_t;
    using s16 = std::int16_t;
    using u16 = std::uint16_t;
    using s32 = std::int32_t;
    using u32 = std::uint32_t;
    using s64 = std::int64_t;
    using u64 = std::uint64_t;

    // floating point types
    using f16 = std::float16_t;
    using bf16 = std::bfloat16_t;
    using f32 = std::float32_t;
    using f64 = std::float64_t;
    using f128 = std::float128_t;

    // character / string types
    using ch = char;
    using str = std::basic_string<ch>;
    using str_view = std::basic_string_view<ch>;

    // path types
    using path = std::filesystem::path;
    constexpr auto aaaaaa = sizeof(path);
}