#include "mwc/input/subsystem.hpp"

namespace mwc {
  namespace input {

    auto input_subsystem_st::initialize() -> void {
      gltf_parser = fastgltf::Parser {fastgltf::Extensions::None};

      m_initialized = true;
    }
    auto input_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
  }
}