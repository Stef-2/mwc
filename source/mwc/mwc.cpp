#include "mwc/mwc.hpp"
#include "mwc/input/subsystem.hpp"

namespace mwc {
  mwc_ct::mwc_ct(const configuration_st& a_configuration)
  : m_window {a_configuration.m_window_configuration},
    m_graphics {m_window, a_configuration.m_version},
    m_configuration {a_configuration} {
    m_window.vkfw_window()->callbacks()->on_key = []([[maybe_unused]] const vkfw::Window& a_window, vkfw::Key a_key,
                                                     [[maybe_unused]] sint32_t a_scancode, vkfw::KeyAction a_key_action,
                                                     [[maybe_unused]] vkfw::ModifierKeyFlags a_modifier_flags) {
      if (a_key_action == vkfw::KeyAction::ePress)
        input::input_subsystem_st::keyboard_st::key_map.emplace(a_key);
      else if (a_key_action == vkfw::KeyAction::eRelease)
        input::input_subsystem_st::keyboard_st::key_map.erase(a_key);
    };

    m_window.vkfw_window()->callbacks()->on_mouse_button = []([[maybe_unused]] const vkfw::Window& a_window,
                                                              vkfw::MouseButton a_button, vkfw::MouseButtonAction a_action,
                                                              [[maybe_unused]] vkfw::ModifierKeyFlags a_modifier_flags) {
      if (a_action == vkfw::MouseButtonAction::ePress)
        input::input_subsystem_st::cursor_st::key_map.emplace(a_button);
      else if (a_action == vkfw::MouseButtonAction::eRelease)
        input::input_subsystem_st::cursor_st::key_map.erase(a_button);
    };
    m_window.vkfw_window()->callbacks()->on_cursor_move = []([[maybe_unused]] const vkfw::Window& a_window,
                                                             const float64_t a_x_position, const float64_t a_y_position) {
      input::input_subsystem_st::cursor_st::x_position = a_x_position;
      input::input_subsystem_st::cursor_st::y_position = a_y_position;
    };
  }
  mwc_ct::~mwc_ct() {
    // if there are any subsystems left unfinalized, finalize them
    for (auto& subsystem : subsystem_st::subsystem_registry)
      if (subsystem->m_initialized) {
        finalize_subsystems();
        return;
      }
  }
}