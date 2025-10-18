#pragma once

#include "mwc/core/contract/definition.hpp"

#include "mwc/core/diagnostic/log/subsystem.hpp"
#include "mwc/window/subsystem.hpp"
#include "mwc/core/chrono/subsystem.hpp"
#include "mwc/core/filesystem/subsystem.hpp"
#include "mwc/core/container/data_span.hpp"
#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"
#include "mwc/graphics/vulkan/pipeline_layout.hpp"
#include "mwc/graphics/vulkan/suballocated_memory_mapped_buffer.hpp"
#include "mwc/input/data/scene/scene.hpp"
#include "mwc/input/data/shader/shader.hpp"

#include <shader-slang/slang.h>
#include <shader-slang/slang-com-ptr.h>

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
        struct scene_processing_configuration_st {
          static constexpr auto default_configuration() -> const scene_processing_configuration_st;

          struct resource_processing_configuration_st {
            bool_t m_cache_data_to_filesystem;
            bool_t m_preserve_in_host_memory;
            bool_t m_propagate_to_device_memory;
          };

          resource_processing_configuration_st m_mesh_processing;
          resource_processing_configuration_st m_image_processing;
          obs_ptr_t<graphics::vulkan::suballocated_memory_mapped_buffer_ct> m_device_buffer;
        };
        struct shader_processing_configuration_st {
          struct device_context_st {
            const obs_ptr_t<graphics::vulkan::logical_device_ct> m_logical_device;
            const obs_ptr_t<graphics::vulkan::physical_device_ct> m_physical_device;
            const obs_ptr_t<graphics::vulkan::pipeline_layout_ct> m_pipeline_layout;
          };
          static constexpr auto default_configuration() -> const shader_processing_configuration_st;

          optional_t<device_context_st> m_device_context;
          bool_t m_cache_spir_v_to_filesystem;
          bool_t m_cache_reflection_data_to_filesystem;
          bool_t m_cache_shader_pipeline_to_filesystem;
        };
        static inline auto gltf_parser = fastgltf::Parser {};
        static inline auto slang_global_session = Slang::ComPtr<slang::IGlobalSession> {};
        static inline auto slang_session = Slang::ComPtr<slang::ISession> {};
        static inline auto scene_registry = vector_t<scene_st> {};
        static inline auto shader_registry = vector_t<shader_st> {};
      };
    };

    [[nodiscard]] auto read_text_file(const file_path_t& a_filepath) -> string_t;
    [[nodiscard]] auto read_binary_file(const file_path_t& a_filepath) -> vector_t<byte_t>;
    auto read_scene_file(const file_path_t& a_filepath,
                         const input_subsystem_st::filesystem_st::scene_processing_configuration_st& a_configuration
                         = input_subsystem_st::filesystem_st::scene_processing_configuration_st::default_configuration()) -> void;
    auto read_shader_file(const file_path_t& a_filepath,
                          const input_subsystem_st::filesystem_st::shader_processing_configuration_st& a_configuration
                          = input_subsystem_st::filesystem_st::shader_processing_configuration_st::default_configuration())
      -> void;

    namespace global {
      inline auto input_subsystem
        = input_subsystem_st {{&diagnostic::log::global::logging_subsystem, &mwc::global::window_subsystem,
                               &chrono::global::chrono_subsystem, &filesystem::global::file_subsystem},
                              string_view_t {"input subsystem"}};
    }

    // implementation
    constexpr auto input_subsystem_st::filesystem_st::scene_processing_configuration_st::default_configuration()
      -> const scene_processing_configuration_st {
      constexpr auto resource_processing_configuration
        = resource_processing_configuration_st {.m_cache_data_to_filesystem = false,
                                                .m_preserve_in_host_memory = true,
                                                .m_propagate_to_device_memory = true};
      return scene_processing_configuration_st {.m_mesh_processing = resource_processing_configuration,
                                                .m_image_processing = resource_processing_configuration,
                                                .m_device_buffer = nullptr};
    }
    constexpr auto input_subsystem_st::filesystem_st::shader_processing_configuration_st::default_configuration()
      -> const shader_processing_configuration_st {
      return shader_processing_configuration_st {.m_device_context = {std::nullopt},
                                                 .m_cache_spir_v_to_filesystem = true,
                                                 .m_cache_reflection_data_to_filesystem = true,
                                                 .m_cache_shader_pipeline_to_filesystem = true};
    }
  }
}
