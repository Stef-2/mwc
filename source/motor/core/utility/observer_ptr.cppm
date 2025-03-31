module;

export module mwc_observer_ptr;

export namespace mwc
{
  template <typename t>
  using observer_ptr_t = t*;
}