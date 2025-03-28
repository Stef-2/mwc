module;

export module mtr_concept;

import std;

export namespace mtr
{
  namespace concepts
  {
    // concept modeling enumerators
    template <typename type>
    concept enumerator = std::is_enum_v<type> or std::is_scoped_enum_v<type>;

    // concept modeling boolean or convertible-to-boolean types
    template <typename type>
    concept boolean =
      std::is_same_v<type, bool> or std::is_convertible_v<type, bool>;

    // concept modeling a predicate which asserts that [type] must be one of the
    // [types]
    template <typename type, typename... types>
    concept any_of = std::disjunction_v<std::is_same<type, types>...>;

    // concept modeling a predicate which asserts that [type] must be the same
    // type as [types]
    template <typename type, typename... types>
    concept all_of = std::conjunction_v<std::is_same<type, types>...>;
  }
}