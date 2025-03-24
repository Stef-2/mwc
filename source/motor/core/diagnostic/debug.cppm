module;

export module mtr_debug;

export namespace mtr
{
    consteval auto debugging() -> const bool
    {
        return {MTR_DEBUG};
    }
}