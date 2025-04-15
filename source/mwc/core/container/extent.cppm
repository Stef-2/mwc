module;

export module mwc_extent;

import mwc_definition;

import std;

export namespace mwc
{
  constexpr auto s_dynamic_extent = std::dynamic_extent;

  template <typename t, size_t n = s_dynamic_extent>
  using extent_t =
    std::conditional_t<n == std::dynamic_extent, vector_t<t>, array_t<t, n>>;
}