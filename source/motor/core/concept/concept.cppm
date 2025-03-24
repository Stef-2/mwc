module;

export module mtr_concept;

import std;

export namespace mtr
{
    namespace concepts
    {
        template <typename T>
        concept enumerator = std::is_enum_v<T> or std::is_scoped_enum_v<T>;
    }
}