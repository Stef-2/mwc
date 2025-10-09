#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/window/subsystem.hpp"
#include "mwc/core/chrono/subsystem.hpp"
#include "mwc/core/container/data_span.hpp"
#include "mwc/graphics/vulkan/suballocated_memory_mapped_buffer.hpp"
#include "mwc/input/data/scene/scene.hpp"

import mwc_definition;
import mwc_subsystem;
import mwc_input_resource;
import mwc_optional;
import mwc_host_mesh;
import mwc_geometry;
import mwc_enum_range;
import mwc_vertex_model;
import mwc_metaprogramming_utility;
import mwc_input_shader_source;

//import vkfw;
import fastgltf;

namespace mwc {
  namespace input {
    struct input_subsystem_st : public subsystem_st {
      using subsystem_st::subsystem_st;

      auto initialize() -> void override final;
      auto finalize() -> void override final;

      static auto poll_hardware_events() -> decltype(vkfw::pollEvents());

      struct keyboard_st {
        static inline auto key_map = unordered_set_t<vkfw::Key> {};
      };
      struct mouse_st {
        static inline auto x_position = float64_t {};
        static inline auto y_position = float64_t {};
        static inline auto key_map = unordered_set_t<vkfw::MouseButton> {};
      };
      struct filesystem_st {
        struct scene_read_configuration_st {
          static constexpr auto default_configuration() -> const scene_read_configuration_st;

          struct resource_processing_configuration_st {
            bool_t m_cache_data_to_filesystem;
            bool_t m_preserve_in_host_memory;
            bool_t m_propagate_to_device_memory;
          };

          resource_processing_configuration_st m_mesh_processing;
          resource_processing_configuration_st m_image_processing;
          obs_ptr_t<graphics::vulkan::suballocated_memory_mapped_buffer_ct> m_device_buffer;
        };
        struct resource_device_memory_st {
          using suballocation_t = graphics::vulkan::suballocated_memory_mapped_buffer_ct::suballocation_t<byte_t>;

          suballocation_t m_memory_mapped_device_mesh_data;
          suballocation_t m_memory_mapped_device_image_data;
          size_t m_source_scene_index;
        };
        static inline auto gltf_parser = fastgltf::Parser {};
        static inline auto scene_registry = vector_t<scene_st> {};
        //static inline auto shader_registry = unordered_set_t<shader_source_st> {};
      };
    };

    auto read_text_file(const file_path_t& a_filepath) -> string_t;
    auto read_binary_file(const file_path_t& a_filepath) -> vector_t<byte_t>;
    auto read_scene_file(const file_path_t& a_filepath,
                         const input_subsystem_st::filesystem_st::scene_read_configuration_st& a_configuration
                         = input_subsystem_st::filesystem_st::scene_read_configuration_st::default_configuration())
      -> optional_t<input_subsystem_st::filesystem_st::resource_device_memory_st>;

    namespace global {
      inline auto input_subsystem = input_subsystem_st {
        {&diagnostic::log::global::logging_subsystem, &mwc::global::window_subsystem, &mwc::chrono::global::chrono_subsystem},
        string_view_t {"input subsystem"}};
    }

    // implementation
    constexpr auto input_subsystem_st::filesystem_st::scene_read_configuration_st::default_configuration()
      -> const scene_read_configuration_st {
      constexpr auto resource_processing_configuration
        = resource_processing_configuration_st {.m_cache_data_to_filesystem = false,
                                                .m_preserve_in_host_memory = true,
                                                .m_propagate_to_device_memory = true};
      return scene_read_configuration_st {.m_mesh_processing = resource_processing_configuration,
                                          .m_image_processing = resource_processing_configuration};
    }
  }
}
