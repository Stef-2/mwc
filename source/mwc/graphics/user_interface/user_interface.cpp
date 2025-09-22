#include "mwc/graphics/user_interface/user_interface.hpp"
#include "mwc/graphics/user_interface/data_render.hpp"
#include "mwc/ecs/definition.hpp"
#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/ecs/subsystem.hpp"
#include "mwc/input/subsystem.hpp"

import mwc_metaprogramming_utility;
import mwc_ctti;

namespace mwc {
  namespace graphics {
    user_interface_ct::user_interface_ct(dear_imgui_ct&& a_dear_imgui, const configuration_st& a_configuration)
    : m_dear_imgui {(std::move(a_dear_imgui))},
      m_configuration {a_configuration} {}
    auto user_interface_ct::begin_frame() const -> void {
      m_dear_imgui.begin_frame();
    }
    auto user_interface_ct::render(const vk::raii::CommandBuffer& a_command_buffer) const -> void {
      m_dear_imgui.render(a_command_buffer);
    }
    auto user_interface_ct::generate_debug_interface() const -> void {
      ImGui::Begin("debug", nullptr);

      // logging
      if constexpr (diagnostic::logging_subsystem_switch()) {
        if (ImGui::BeginChild("logging", ImVec2 {0.0, ImGui::GetFrameHeight() * (10 + 1)}, ImGuiChildFlags_Borders)) {
          dear_imgui_ct::centered_text("logging");
          const auto begin_ptr = diagnostic::log::global::logging_subsystem.string_sink.c_str();
          const auto end_ptr = begin_ptr + diagnostic::log::global::logging_subsystem.string_sink.size();
          ImGui::TextUnformatted(begin_ptr, end_ptr);
        }
        ImGui::EndChild();
      }
      // input
      if (ImGui::BeginChild("input", ImVec2 {0.0, ImGui::GetFrameHeight() * (3 + 1)}, ImGuiChildFlags_Borders)) {
        dear_imgui_ct::centered_text("input");
        ImGui::Text("keyboard: ");
        for (const auto& pressed_keyboard_key : input::input_subsystem_st::keyboard_st::key_map) {
          const auto key_name = vkfw::getKeyName(pressed_keyboard_key).value;

          ImGui::SameLine();
          ImGui::SmallButton(key_name.data() ? key_name.data()
                                             : std::to_string(std::to_underlying(pressed_keyboard_key)).c_str());
        }
        ImGui::TextUnformatted(std::format("cursor: {0} - {1}",
                                           input::input_subsystem_st::mouse_st::x_position,
                                           input::input_subsystem_st::mouse_st::y_position)
                                 .c_str());
        for (const auto& pressed_mouse_key : input::input_subsystem_st::mouse_st::key_map) {
          ImGui::SameLine();
          ImGui::SmallButton(std::to_string(std::to_underlying(pressed_mouse_key)).c_str());
        }
      }
      ImGui::EndChild();
      // ecs
      if (ImGui::BeginChild("ecs", ImVec2 {0.0, 640.0}, ImGuiChildFlags_Borders)) {
        dear_imgui_ct::centered_text("ecs");
        using component_tuple_t = typename decltype(ctti::observe_type_list<ecs::component_type_list_st>())::component_tuple_t;
        constexpr auto component_count = std::tuple_size_v<component_tuple_t>;

        constexpr auto table_flags = ImGuiTableFlags_Borders bitor ImGuiTableFlags_HighlightHoveredColumn
                               bitor ImGuiTableFlags_ScrollX bitor ImGuiTableFlags_ScrollY bitor ImGuiTableFlags_RowBg
                               bitor ImGuiTableFlags_SizingFixedSame;
        constexpr auto table_size = ImVec2 {0, 480};
        ImGui::BeginTable("ecs", component_count + 1, table_flags, table_size);
        ImGui::TableSetupColumn("entity");
        mwc::static_for_loop<0, component_count>([]<size_t tp_index> {
          constexpr auto header_string = string_t {std::tuple_element_t<tp_index, component_tuple_t>::type_name(false)};
          ImGui::TableSetupColumn(header_string.c_str());
        });
        ImGui::TableHeadersRow();

        const auto lambda = []<size_t tp_index, size_t... tp_indices>(this auto&& a_this,
                                                                      std::index_sequence<tp_index, tp_indices...>) {
          const auto row_index = ImGui::TableGetRowIndex() - 1;
          auto entity_archetype_record = ecs::ecs_subsystem_st::entity_archetype_map.begin();
          std::advance(entity_archetype_record, row_index);

          if constexpr (tp_index == 0) {
            ImGui::TableSetColumnIndex(0);
            const auto entity_index_string = std::format("{0}", entity_archetype_record->first);
            dear_imgui_ct::centered_text(entity_index_string);
            ImGui::OpenPopupOnItemClick(std::bit_cast<const char*>(&entity_archetype_record->first),
                                        ImGuiPopupFlags_MouseButtonLeft);
            if (ImGui::BeginPopup(std::bit_cast<const char*>(&entity_archetype_record->first))) {
              const auto entity_archetype_string
                = std::format("located in archetype:\n\tindex: {0}\n\tcomponent hash: {1}\n\tentity row: {2}",
                              entity_archetype_record->second.m_archetype->index(),
                              entity_archetype_record->second.m_archetype->component_hash(),
                              entity_archetype_record->second.m_entity_index);
              ImGui::TextUnformatted(entity_archetype_string.c_str());
              ImGui::EndPopup();
            }
            if constexpr (sizeof...(tp_indices) > 0)
              a_this(std::index_sequence<tp_indices...> {});
            else
              return;
          } else {
            if (entity_archetype_record == ecs::ecs_subsystem_st::entity_archetype_map.end())
              return;

            using component_t = std::tuple_element_t<tp_index - 1, component_tuple_t>;

            ImGui::TableSetColumnIndex(tp_index);

            const auto archetype_component_index
              = entity_archetype_record->second.m_archetype->component_index(component_t::index);
            if (archetype_component_index != ecs::null_archetype_component_index) {
              const auto component_vector_ptr = std::bit_cast<vector_t<component_t>*>(
                &entity_archetype_record->second.m_archetype->m_component_data[archetype_component_index].m_data);
              const auto component_data_ptr = component_vector_ptr->operator[](entity_archetype_record->second.m_entity_index);
              const auto component_data_string = format_component_data(component_data_ptr);
              dear_imgui_ct::centered_text(component_data_string);
              ImGui::OpenPopupOnItemClick(std::bit_cast<const char*>(&component_data_ptr), ImGuiPopupFlags_MouseButtonLeft);
              if (ImGui::BeginPopup(std::bit_cast<const char*>(&component_data_ptr))) {
                const auto component_archetype_string
                  = std::format("located in archetype:\n\tindex: {0}\n\tcomponent hash: {1}\n\tcomponent column: {2}",
                                entity_archetype_record->second.m_archetype->index(),
                                entity_archetype_record->second.m_archetype->component_hash(),
                                archetype_component_index);
                ImGui::TextUnformatted(component_archetype_string.c_str());
                ImGui::EndPopup();
              }
            }

            if constexpr (sizeof...(tp_indices) > 0)
              a_this(std::index_sequence<tp_indices...> {});
          }
        };
        for (auto i = ecs::archetype_entity_index_t {0}; i < ecs::ecs_subsystem_st::entity_archetype_map.size(); ++i) {
          ImGui::TableNextRow();
          lambda(std::make_index_sequence<component_count + 1> {});
        }

        ImGui::EndTable();
      }
      ImGui::EndChild();

      ImGui::End();
    }
  }
}