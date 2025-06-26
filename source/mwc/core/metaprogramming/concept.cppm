module;

export module mwc_concept;

import mwc_definition;

import std;

export namespace mwc {
  namespace concepts {
    // concept modeling enumerator types
    template <typename tp>
    concept enumerator_c = std::is_enum_v<tp> or std::is_scoped_enum_v<tp>;

    // concept modeling enumerator types with a terminating [end] value
    template <typename tp>
    concept end_terminated_enum_c = enumerator_c<tp> and requires { tp::end; };

    // concept modeling [end] terminated enumerator types with strictly and steadily increasing monotonic values
    template <typename tp>
    concept steady_monotonic_enum_c =
      end_terminated_enum_c<tp> and requires {
                                      std::invoke([] {
                                        for (auto i = std::underlying_type_t<tp> {0}; i < std::to_underlying(tp::end); ++i)
                                          [[maybe_unused]]
                                          const auto enumerator = tp {i};
                                      });
                                    };
    // concept modeling boolean or boolean convertible types
    template <typename tp>
    concept bool_tean_c = std::is_same_v<tp, bool_t> or std::is_convertible_v<tp, bool_t>;

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