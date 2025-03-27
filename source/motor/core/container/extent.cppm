module;

export module mtr_extent;

import mtr_definition;

import std;

export namespace mtr
{
  template <typename t, size_t n = std::dynamic_extent>
  using extent_t =
  std::conditional_t<n == std::dynamic_extent, vector_t<t>, array_t<t, n>>;
}