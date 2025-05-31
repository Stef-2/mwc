module;

#include "mwc/core/definition/definition.hpp"


#include <type_traits>
#include <span>

export module mwc_extent;

export namespace mwc {
  template <typename tp, size_t tp_size = std::dynamic_extent>
  using extent_t = std::conditional_t<tp_size == std::dynamic_extent,
                                      vector_t<tp>, array_t<tp, tp_size>>;
}