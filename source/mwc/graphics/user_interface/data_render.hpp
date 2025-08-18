#pragma once

#include "mwc/core/utility/pointer_cast.hpp"

#include "imgui.h"

import mwc_definition;
import mwc_metaprogramming_utility;

import std;

/*

namespace mwc {
  namespace graphics {
    template <typename tp_container>
    auto render_data(const tp_container& a_container, const string_view_t a_name) -> void {
      // sequence containers
      if constexpr (std::contiguous_iterator<typename tp_container::iterator>) {
        if (ImGui::CollapsingHeader(a_name.data())) {
          ImGui::PushID(&a_container);
          const auto header = std::format("address:{0:#X} size: {1} bytes: {2}", pointer_cast(a_container.data()),
                                          a_container.size(), a_container.size() * sizeof(typename tp_container::value_type));
          ImGui::Text(header.c_str());
          for (auto i = 0uz; i < a_container.size(); ++i) {
            const auto data = std::format("[{}]    {}", i, a_container[i]);
            ImGui::Text(data.c_str());
          }
          ImGui::PopID();
        }
      }
      // associative containers
      else if (ImGui::CollapsingHeader(a_name.data())) {
        ImGui::PushID(&a_container);
        const auto header = std::format("size: {0}", a_container.size());
        ImGui::Text(header.c_str());
        for (auto i = 0uz; const auto& [key, value] : a_container) {
          const auto data = std::format("[{0}]    {1} - {2}", i, key, value);
          ImGui::Text(data.c_str());
        }
        ImGui::PopID();
      }
    }
  }
}*/