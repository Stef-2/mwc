module;

export module mtr_observer_ptr;

export namespace mtr
{
  template <typename t>
  using observer_ptr_t = t*;
}