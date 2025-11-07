#include "mwc/mwc.hpp"
#include "mwc/input/subsystem.hpp"

namespace mwc {
  mwc_ct::mwc_ct(const configuration_st& a_configuration)
  : m_window {a_configuration.m_window_configuration},
    m_graphics {m_window, a_configuration.m_version},
    m_configuration {a_configuration} {
    m_window.vkfw_window()->callbacks()->on_key
      = []([[maybe_unused]] const vkfw::Window& a_window, vkfw::Key a_key, [[maybe_unused]] sint32_t a_scancode,
           vkfw::KeyAction a_key_action, [[maybe_unused]] vkfw::ModifierKeyFlags a_modifier_flags) {
          // note: remove this later
          if (a_key == vkfw::Key::eEscape)
            std::quick_exit(0);
          if (a_key_action == vkfw::KeyAction::ePress)
            input::input_subsystem_st::keyboard_st::key_map.emplace(a_key);
          else if (a_key_action == vkfw::KeyAction::eRelease)
            input::input_subsystem_st::keyboard_st::key_map.erase(a_key);
        };

    m_window.vkfw_window()->callbacks()->on_mouse_button
      = []([[maybe_unused]] const vkfw::Window& a_window, vkfw::MouseButton a_button, vkfw::MouseButtonAction a_action,
           [[maybe_unused]] vkfw::ModifierKeyFlags a_modifier_flags) {
          if (a_action == vkfw::MouseButtonAction::ePress)
            input::input_subsystem_st::mouse_st::key_map.emplace(a_button);
          else if (a_action == vkfw::MouseButtonAction::eRelease)
            input::input_subsystem_st::mouse_st::key_map.erase(a_button);
        };

    /*
    m_window.vkfw_window()->callbacks()->on_cursor_move
      = []([[maybe_unused]] const vkfw::Window& a_window, const float64_t a_cursor_x_position,
           const float64_t a_cursor_y_position) {
          input::input_subsystem_st::mouse_st::previous_cursor_position
            = input::input_subsystem_st::mouse_st::current_cursor_position;
          input::input_subsystem_st::mouse_st::current_cursor_position.m_x = a_cursor_x_position;
          input::input_subsystem_st::mouse_st::current_cursor_position.m_y = a_cursor_y_position;
        };*/
  }
  mwc_ct::~mwc_ct() {
    // if there are any subsystems left unfinalized, finalize them
    for (auto& subsystem : subsystem_st::subsystem_registry)
      if (subsystem->m_initialized) {
        finalize_subsystems();
        return;
      }
  }
  auto mwc_ct::run() -> void {
    while (true) {
      std::ignore = mwc::input::input_subsystem_st::poll_hardware_events();

      auto cursor_position_x = input::input_subsystem_st::mouse_st::cursor_positon_st::scalar_t {};
      auto cursor_position_y = input::input_subsystem_st::mouse_st::cursor_positon_st::scalar_t {};
      m_window.vkfw_window()->getCursorPos(&cursor_position_x, &cursor_position_y);

      input::input_subsystem_st::mouse_st::previous_cursor_position
        = input::input_subsystem_st::mouse_st::current_cursor_position;
      input::input_subsystem_st::mouse_st::current_cursor_position.m_x = cursor_position_x;
      input::input_subsystem_st::mouse_st::current_cursor_position.m_y = cursor_position_y;

      m_graphics.render();
    }
  }
}