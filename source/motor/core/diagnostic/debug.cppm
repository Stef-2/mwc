module;

export module mtr_debug;

export namespace mtr
{
  // debugging mode
  consteval auto debugging() -> const bool { return {MTR_DEBUG}; }
}