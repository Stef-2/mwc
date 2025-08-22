#pragma once

#include "mwc/ecs/component.hpp"
#include "imgui.h"

import mwc_definition;
import mwc_metaprogramming_utility;

import std;

// byte_t formatter
template <>
struct std::formatter<mwc::byte_t, mwc::char_t> {
  constexpr auto parse(const std::format_parse_context& a_context) {
    return a_context.begin();
  }
  constexpr auto format(const mwc::byte_t a_byte, std::format_context& a_context) const {
    return std::format_to(a_context.out(), "{0}", std::to_integer<mwc::size_t>(a_byte));
  }
};

namespace mwc {
  namespace graphics {
    // format component
    template <mwc::ecs::component_c tp_component>
    constexpr auto format_component_data(tp_component a_component) {
      using underlying_pod_t = std::remove_cvref_t<tp_component>::underlying_pod_t;
      constexpr auto format_elements_per_line = size_t {4};

      auto buffer = mwc::string_t {};

      // determine if the component's underlying plain old data type is formattable
      if constexpr (std::formattable<underlying_pod_t, mwc::char_t>) {
        // determine if the component's underlying plain old data is an array
        if constexpr (sizeof(tp_component) > sizeof(underlying_pod_t) and sizeof(tp_component) % sizeof(underlying_pod_t) == 0) {
          auto array_ptr = std::bit_cast<underlying_pod_t*>(&a_component);
          // format each element of the array
          static_for_loop<0, sizeof(tp_component) / sizeof(underlying_pod_t)>([&buffer, &array_ptr]<size_t tp_index> {
            if constexpr (tp_index > 0 and tp_index % format_elements_per_line == 0)
              buffer.append("\n");
            std::format_to(std::back_inserter(buffer), "{0} ", array_ptr[tp_index]);
          });
        } else {
          return std::format("{0}", *std::bit_cast<underlying_pod_t*>(&a_component));
        }
      } else {
        // component data is not formattable as a type, format it as an array of bytes
        const auto byte_ptr = std::bit_cast<mwc::byte_t*>(&a_component);
        static_for_loop<0, sizeof(tp_component)>([&buffer, &byte_ptr]<size_t tp_index> {
          if constexpr (tp_index > 0 and tp_index % format_elements_per_line == 0)
            buffer.append("\n");
          std::format_to(std::back_inserter(buffer), "{0} ", byte_ptr[tp_index]);
        });
      }

      return buffer;
    }
  }
}