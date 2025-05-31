module;

export module mwc_observer_ptr;

export namespace mwc {
  template <typename tp>
  using observer_ptr_t = tp*;
}