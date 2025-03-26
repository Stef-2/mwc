module;

export module mtr_concept;

import std;

export namespace mtr
{
    namespace concepts
    {
        template <typename t>
        concept enumerator = std::is_enum_v<t> or std::is_scoped_enum_v<t>;

        template <typename t>
        concept boolean = std::is_same_v<t, bool> or std::is_convertible_v<t, bool>;
    }
}