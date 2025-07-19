module;

export module mwc_consteval_counter;

export namespace mwc {
  template <auto, auto>
  struct ADLType {
    friend consteval auto ADLFunc(ADLType, auto...);
  };

  template <auto InstanceIdentifier, auto x>
  struct InjectDefinitionForADLFunc {
    friend consteval auto ADLFunc(ADLType<InstanceIdentifier, x>, auto...) {}
  };

  template <auto InstanceIdentifier, auto x = 0, auto = [] {}>
  consteval auto ExtractThenUpdateCurrentState() -> decltype(x) {
    if constexpr (requires { ADLFunc(ADLType<InstanceIdentifier, x> {}); })
      return ExtractThenUpdateCurrentState<InstanceIdentifier, x + 1>();
    else
      InjectDefinitionForADLFunc<InstanceIdentifier, x> {};
    return x;
  }

  template <auto InstanceIdentifier, auto x = 0, auto = [] {}>
  consteval auto PeekCurrentState() -> decltype(x) {
    if constexpr (requires { ADLFunc(ADLType<InstanceIdentifier, x> {}); })
      return PeekCurrentState<InstanceIdentifier, x + 1>();
    else
      return x;
  }
}