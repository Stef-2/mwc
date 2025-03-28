module;

export module mtr_concept;

import std;

export namespace mtr
{
  namespace concepts
  {
    template <typename type>
    concept enumerator = std::is_enum_v<type> or std::is_scoped_enum_v<type>;

    template <typename type>
    concept boolean =
      std::is_same_v<type, bool> or std::is_convertible_v<type, bool>;

    template <typename type, typename... types>
    concept is_any = std::disjunction_v<std::is_same<type, types>...>;

    template <typename type, typename... types>
    concept are_same = std::conjunction_v<std::is_same<type, types>...>;
  }
}