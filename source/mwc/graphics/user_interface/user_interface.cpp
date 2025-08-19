#include "mwc/graphics/user_interface/user_interface.hpp"
#include "mwc/graphics/user_interface/data_render.hpp"
#include "mwc/ecs/definition.hpp"
#include "mwc/ecs/subsystem.hpp"

import mwc_metaprogramming_utility;
import mwc_ctti;

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

namespace {
  template <mwc::ecs::component_c tp_component>
  constexpr auto format(tp_component a_component) {
    using underlying_pod_t = std::remove_cvref_t<tp_component>::underlying_pod_t;

    auto buffer = mwc::string_t {};

    if constexpr (std::formattable<underlying_pod_t, mwc::char_t>) {
      if constexpr (sizeof(tp_component) > sizeof(underlying_pod_t)) {
        auto ptr = std::bit_cast<underlying_pod_t*>(&a_component);
        for (auto i = 0; i < sizeof(tp_component) / sizeof(underlying_pod_t); ++i) {
          std::format_to(std::back_inserter(buffer), "{0}", ptr[i]);
        }
      } else {
        return std::format("{0}", *std::bit_cast<underlying_pod_t*>(&a_component));
      }
    } else {
      const auto bytes = std::bit_cast<mwc::byte_t*>(&a_component);
      for (auto i = 0; i < sizeof(underlying_pod_t); ++i) {
        std::format_to(std::back_inserter(buffer), "{0}", bytes[i]);
      }
    }

    return buffer;
  }
}

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
      auto open = true;
      ImGui::Begin("debug", &open);

      using component_tuple_t = typename decltype(ctti::observe_type_list<ecs::component_type_list_st>())::component_tuple_t;
      constexpr auto component_count = std::tuple_size_v<component_tuple_t>;

      constexpr auto table_flags = ImGuiTableFlags_Borders bitor ImGuiTableFlags_HighlightHoveredColumn bitor
                                   ImGuiTableFlags_ScrollX bitor ImGuiTableFlags_ScrollY bitor ImGuiTableFlags_RowBg bitor
                                   ImGuiTableFlags_SizingFixedSame;
      constexpr auto table_size = ImVec2 {640, 480};
      ImGui::BeginTable("ecs", component_count + 1, table_flags, table_size);
      mwc::static_for_loop<0, component_count>([]<size_t tp_index> {
        constexpr auto header_string = string_t {std::tuple_element_t<tp_index, component_tuple_t>::type_name()};
        ImGui::TableSetupColumn(header_string.c_str());
      });
      ImGui::TableHeadersRow();

      const auto lambda = []<size_t tp_index, size_t... tp_indices>(this auto&& a_this,
                                                                    std::index_sequence<tp_index, tp_indices...>) {
        using component_t = std::tuple_element_t<tp_index, component_tuple_t>;

        const auto row_index = ImGui::TableGetRowIndex() - 1;
        auto entity_archetype_record = ecs::ecs_subsystem_st::entity_archetype_map.begin();
        std::advance(entity_archetype_record, row_index);
        if (entity_archetype_record == ecs::ecs_subsystem_st::entity_archetype_map.end())
          return;

        ImGui::TableSetColumnIndex(tp_index);
        const auto archetype_component_index = entity_archetype_record->second.m_archetype->component_index(component_t::index);
        if (archetype_component_index != ecs::null_archetype_component_index) {
          const auto component_data_ptr = std::bit_cast<vector_t<component_t>*>(
            &entity_archetype_record->second.m_archetype->m_component_data[archetype_component_index].m_data);
          ImGui::Text(format(component_data_ptr->operator[](entity_archetype_record->second.m_entity_index)).c_str());
        }
        /*
        ImGui::TableSetColumnIndex(tp_index + 1);

        constexpr auto button_label = string_t {std::tuple_element_t<tp_index, component_tuple_t>::type_name()};
        ImGui::Button(button_label.c_str(), {60, 20});
        const auto equal_range = ecs::ecs_subsystem_st::component_archetype_map.equal_range(component_t::index);

        auto header_string = string_t {"present in the following archetypes:\n"};
        for (const auto& [component_index, archetype_component_column_map] :
             std::ranges::subrange(equal_range.first, equal_range.second)) {
          for (const auto& [archetype_ptr, component_column_index] : archetype_component_column_map) {
            std::format_to(std::back_inserter(header_string), "index: {0}, hash: {1}, component column: {2}\n",
                           archetype_ptr->index(), archetype_ptr->component_hash(), component_column_index);
            for (const auto& [entity_index, archetype_entity_map] : ecs::ecs_subsystem_st::entity_archetype_map) {
              ImGui::TableNextRow();

              if (archetype_ptr == archetype_entity_map.m_archetype) {
                const auto component_data_ptr =
                  &archetype_ptr->m_component_data[component_column_index].m_data[archetype_entity_map.m_entity_index];
                const auto component_ptr = std::bit_cast<component_t*>(component_data_ptr);
                ImGui::Text(format(*component_ptr).c_str());
              }
            }
          }
        }
        if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
          ImGui::Text(header_string.c_str());
          ImGui::EndPopup();
        }*/

        if constexpr (sizeof...(tp_indices) > 0)
          a_this(std::index_sequence<tp_indices...> {});
      };
      for (auto i = ecs::archetype_entity_index_t {0}; i < ecs::ecs_subsystem_st::entity_archetype_map.size(); ++i) {
        ImGui::TableNextRow();
        lambda(std::make_index_sequence<component_count> {});
      }
      /*
      for (const auto& [entity_index, archetype_entity_index] : ecs::ecs_subsystem_st::entity_archetype_map) {
        auto& archetype = *archetype_entity_index.m_archetype;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Button(std::to_string(entity_index).c_str(), {60, 20});
        if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft)) {
          const auto archetype_information =
            std::format("present in the following archetype: index: {0}, hash {1}, entity row: {2}",
                        archetype.index(),
                        archetype.component_hash(),
                        archetype_entity_index.m_entity_index);
          ImGui::Text(archetype_information.c_str());
          ImGui::EndPopup();
        }
        const auto component_row_data = archetype.component_data_row(archetype_entity_index.m_entity_index);
        for (auto i = ecs::archetype_entity_index_t {0}; i < component_row_data.size(); ++i) {
          ImGui::TableSetColumnIndex(archetype.m_component_data[i].m_component_index);
          auto buffer = string_t {};
          for (const auto& byte : component_row_data[i])
            std::format_to(std::back_inserter(buffer), "{0} ", byte);
          ImGui::TextWrapped(buffer.c_str());
        }
      }
*/
      ImGui::EndTable();
      ImGui::End();
    }
  }
}