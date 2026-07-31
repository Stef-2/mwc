module;

export module mwc_incomplete_type;

import mwc_definition;

export namespace mwc {
  template <typename tp>
  struct incomplete_type_t_st;
  template <size_t tp, size_t tp_generation = size_t {0}>
  struct incomplete_type_v_st;
}