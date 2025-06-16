module;

export module mwc_observer_ptr;

export namespace mwc {
  template <typename tp>
  using obs_ptr_t = tp*;
}