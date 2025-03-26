module;

export module mtr_enum_bitwise_operators;

import mtr_concept;

import std;

export namespace mtr
{
    constexpr auto operator not(const concepts::enumerator auto x)
    {
        return static_cast<const concepts::enumerator auto>(not std::to_underlying(x));
    }

    constexpr auto operator bitor(const concepts::enumerator auto x, const concepts::enumerator auto y)
    {
        return static_cast<const concepts::enumerator auto>(std::to_underlying(x) bitor std::to_underlying(y));
    }

    constexpr auto operator bitand(const concepts::enumerator auto x, const concepts::enumerator auto y)
    {
        return static_cast<const concepts::enumerator auto>(std::to_underlying(x) bitand std::to_underlying(y));
    }

    constexpr auto operator xor(const concepts::enumerator auto x, const concepts::enumerator auto y)
    {
        return static_cast<const concepts::enumerator auto>(std::to_underlying(x) xor std::to_underlying(y));
    }

    constexpr auto operator|=(concepts::enumerator auto &x, const concepts::enumerator auto y)
    {
        return x bitor y;
    }

    constexpr auto operator&=(concepts::enumerator auto &x, const concepts::enumerator auto y)
    {
        return x bitand y;
    }

    constexpr auto operator^=(concepts::enumerator auto &x, const concepts::enumerator auto y)
    {
        return x xor y;
    }
}