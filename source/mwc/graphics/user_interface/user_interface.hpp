#pragma once

#include "mwc/graphics/user_interface/dear_imgui.hpp"

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    class user_interface_ct {
      public:
      struct configuration_st {
        static constexpr auto default_configuration() -> configuration_st;
      };

      user_interface_ct(dear_imgui_ct&& a_dear_imgui,
                        const configuration_st& a_configuration = configuration_st::default_configuration());

      auto begin_frame() const -> void;
      auto render(const vk::raii::CommandBuffer& a_command_buffer) const -> void;

      auto generate_debug_interface() const -> void;

      private:
      dear_imgui_ct m_dear_imgui;
      configuration_st m_configuration;
    };

    // implementation
    constexpr auto user_interface_ct::configuration_st::default_configuration() -> configuration_st {
      return configuration_st {};
    }
  }
}
