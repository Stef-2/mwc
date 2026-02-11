module;

#include <shader-slang/slang.h>
#include <shader-slang/slang-com-ptr.h>

export module mwc_slang;

export namespace Slang {
  using Slang::ComPtr;
}
export namespace slang {
  using slang::CompilerOptionEntry;
  using slang::CompilerOptionName;
  using slang::CompilerOptionValue;
  using slang::CompilerOptionValueKind;
  using slang::createGlobalSession;
  using slang::IBlob;
  using slang::IComponentType;
  using slang::IEntryPoint;
  using slang::IGlobalSession;
  using slang::IMetadata;
  using slang::IModule;
  using slang::ISession;
  using slang::SessionDesc;
  using slang::TargetDesc;
}

export {
  using ::SlangCompileTarget;
  using ::SlangMatrixLayoutMode;
  using ::SlangStage;

  constexpr auto slang_succeeded(const SlangResult a_status) -> bool {
    return SLANG_SUCCEEDED(a_status);
  }
}