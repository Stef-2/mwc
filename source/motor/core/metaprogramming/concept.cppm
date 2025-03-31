module;

export module mwc_concept;

import std;

export namespace mwc
{
  namespace concepts
  {
    // concept modeling enumerator_cs
    template <typename type>
    concept enumerator_c = std::is_enum_v<type> or std::is_scoped_enum_v<type>;

    // concept modeling boolean_c or convertible-to-boolean_c types
    template <typename type>
    concept boolean_c =
      std::is_same_v<type, bool> or std::is_convertible_v<type, bool>;

    // concept modeling a predicate which asserts that [type] must be one of the
    // [types]
    template <typename type, typename... types>
    concept any_of_c = std::disjunction_v<std::is_same<type, types>...>;

    // concept modeling a predicate which asserts that [type] must be the same
    // type as [types]
    template <typename type, typename... types>
    concept all_of_c = std::conjunction_v<std::is_same<type, types>...>;
  }
}