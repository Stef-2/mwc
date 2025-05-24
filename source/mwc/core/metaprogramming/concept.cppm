module;

export module mwc_concept;

import std;

export namespace mwc {
  namespace concepts {
    // concept modeling enumerator types
    template <typename tp>
    concept enumerator_c = std::is_enum_v<tp> or std::is_scoped_enum_v<tp>;

    // concept modeling boolean or boolean-convertible types
    template <typename tp>
    concept boolean_c =
      std::is_same_v<tp, bool> or std::is_convertible_v<tp, bool>;

    // concept modeling a predicate that asserts that [tp] must be one of the [tps]
    template <typename tp, typename... tps>
    concept any_of_c = std::disjunction_v<std::is_same<tp, tps>...>;

    // concept modeling a predicate that asserts that [tp] must be the same as [tps]
    template <typename tp, typename... tps>
    concept all_of_c = std::conjunction_v<std::is_same<tp, tps>...>;

    // concept modeling storage types that provide [data] and [size] functions
    template <typename tp>
    concept contiguous_storage_c = requires(tp a_value) {
                                     a_value.data();
                                     a_value.size();
                                   };
  }
}