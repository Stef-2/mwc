#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/container/static_bi_map.hpp"

import std;

namespace mwc {
  enum class file_type_et {
    e_binary,
    e_text,
    e_scene,
    e_shader_source,
    e_spir_v,
    e_shader_object,
    end
  };

  constexpr auto file_extension_string_map(const auto a_identity)
    requires(concepts::any_of_c<std::remove_cvref_t<decltype(a_identity)>, file_type_et, string_view_t>) {
    using enum file_type_et;

    static constexpr auto map = static_unordered_bi_map_st {{pair_t<file_type_et, string_view_t> {e_binary, ".bin"},
                                                             {e_text, ".txt"},
                                                             {e_scene, ".glb"},
                                                             {e_shader_source, ".slang"},
                                                             {e_spir_v, ".spirv"},
                                                             {e_shader_object, ".sobj"}}};

    return map[a_identity];
  }
}