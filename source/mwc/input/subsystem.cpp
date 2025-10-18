#include "mwc/input/subsystem.hpp"
#include "mwc/core/filesystem/subsystem.hpp"
#include "mwc/graphics/vulkan/shader_object.hpp"
#include "mwc/output/output.hpp"

#include <stb/stb_image.h>

import mwc_breakpoint;
namespace {
  auto process_slang_diagnostics(const Slang::ComPtr<slang::IBlob> a_slang_diagnostics) -> void {
    if (a_slang_diagnostics) {
      mwc::warning(
        std::format("slang diagnostics: {0}\n", static_cast<const mwc::char_t*>(a_slang_diagnostics->getBufferPointer())));
    }
  }
  constexpr auto vulkan_shader_stage(const SlangStage a_stage) -> vk::ShaderStageFlagBits {
    using enum vk::ShaderStageFlagBits;
    switch (a_stage) {
      case SLANG_STAGE_VERTEX : return eVertex;
      case SLANG_STAGE_FRAGMENT : return eFragment;
      case SLANG_STAGE_COMPUTE : return eCompute;
      default : contract_assert(false); std::unreachable();
    }
  }
}
namespace mwc {
  namespace input {
    auto input_subsystem_st::initialize() -> void {
      keyboard_st::key_map.reserve(std::to_underlying(vkfw::Key::eLAST));
      mouse_st::key_map.reserve(std::to_underlying(vkfw::MouseButton::eLAST));
      filesystem_st::gltf_parser = fastgltf::Parser {fastgltf::Extensions::None};

      // slang configuration
      const auto slang_global_session_generation_result
        = slang::createGlobalSession(filesystem_st::slang_global_session.writeRef());
      contract_assert(SLANG_SUCCEEDED(slang_global_session_generation_result));
      const auto& shader_data_directory_path = filesystem::directory(filesystem::directory_et::e_shader).c_str();
      const auto slang_target_description
        = slang::TargetDesc {.format = SLANG_SPIRV, .profile = filesystem_st::slang_global_session->findProfile("spirv_1_6")};
      auto slang_compiler_options = array_t<slang::CompilerOptionEntry, 1> {
        slang::CompilerOptionEntry {slang::CompilerOptionName::EmitSpirvDirectly,
                                    slang::CompilerOptionValue {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}}};
      const auto slang_session_description = slang::SessionDesc {.targets = &slang_target_description,
                                                                 .targetCount = 1,
                                                                 .flags = {},
                                                                 .defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
                                                                 .searchPaths = &shader_data_directory_path,
                                                                 .searchPathCount = 1,
                                                                 .compilerOptionEntries = slang_compiler_options.data(),
                                                                 .compilerOptionEntryCount = slang_compiler_options.size()};
      const auto slang_session_generation_result
        = filesystem_st::slang_global_session->createSession(slang_session_description, filesystem_st::slang_session.writeRef());
      contract_assert(SLANG_SUCCEEDED(slang_session_generation_result));

      m_initialized = true;
    }
    auto input_subsystem_st::finalize() -> void {
      m_initialized = false;
    }
    auto input_subsystem_st::poll_hardware_events() -> decltype(vkfw::pollEvents()) {
      return vkfw::pollEvents();
    }
    auto read_text_file(const file_path_t& a_filepath) -> string_t {
      auto input_file_stream = std::ifstream {a_filepath, std::ios::in};
      contract_assert(input_file_stream);
      auto buffer = std::stringstream {};

      buffer << input_file_stream.rdbuf();
      input_file_stream.close();

      return buffer.str();
    }
    auto read_binary_file(const file_path_t& a_filepath) -> vector_t<byte_t> {
      auto input_file_stream = std::ifstream {a_filepath, std::ios::binary bitor std::ios::in};
      contract_assert(input_file_stream);

      const auto file_size = std::filesystem::file_size(a_filepath);
      auto buffer = vector_t<byte_t>(file_size);

      input_file_stream.read(std::bit_cast<obs_ptr_t<char_t>>(buffer.data()), file_size);
      input_file_stream.close();

      return buffer;
    }
    auto read_scene_file(const file_path_t& a_filepath,
                         const input_subsystem_st::filesystem_st::scene_processing_configuration_st& a_configuration) -> void {
      // sanity checks
      // scene mesh and image data should either be preserved in host memory or propagated to device memory, or both
      if (not a_configuration.m_mesh_processing.m_preserve_in_host_memory
          and not a_configuration.m_mesh_processing.m_propagate_to_device_memory)
        warning(std::format("scene read from {0} is not configured for mesh processing", a_filepath.c_str()));
      if (not a_configuration.m_image_processing.m_preserve_in_host_memory
          and not a_configuration.m_image_processing.m_propagate_to_device_memory)
        warning(std::format("scene read from {0} is not configured for image processing", a_filepath.c_str()));

      // scene configured for device memory processing should have a virtual memory suballocator provided and vice versa
      const auto device_memory_processing_requested = a_configuration.m_mesh_processing.m_propagate_to_device_memory
                                                   or a_configuration.m_image_processing.m_propagate_to_device_memory;
      if (device_memory_processing_requested and not a_configuration.m_device_buffer)
        error(
          std::format("scene read from {0} is configured for device memory processing, but no virtual allocator was provided",
                      a_filepath.c_str()));
      if (a_configuration.m_device_buffer and not device_memory_processing_requested)
        warning(std::format(
          "scene read from {0} is not configured for device memory processing, but a virtual allocator was provided",
          a_filepath.c_str()));

      auto [data_buffer_error, data_buffer] = fastgltf::GltfDataBuffer::FromPath(a_filepath);
      contract_assert(data_buffer_error == fastgltf::Error::None);

      constexpr auto parse_options = fastgltf::Options::GenerateMeshIndices;
      auto&& [parse_error, asset]
        = input_subsystem_st::filesystem_st::gltf_parser.loadGltfBinary(data_buffer, a_filepath, parse_options);
      contract_assert(parse_error == fastgltf::Error::None);
      // assert one scene per file
      contract_assert(asset.scenes.size() == 1);

      const auto& gltf_scene = asset.scenes.front();
      auto native_scene = scene_st {};
      native_scene.m_nodes = decltype(scene_st::m_nodes)::configuration_st {asset.nodes.size()},
      native_scene.m_meshes.reserve(asset.meshes.size());
      native_scene.m_images.reserve(asset.images.size());

      // initial mesh processing
      using index_t = std::remove_cvref_t<decltype(native_scene.m_meshes.front().m_host_mesh.m_index_storage.front())>;
      constexpr auto index_byte_count = sizeof(index_t);
      auto mesh_accessor_offset_pairs = vector_t<pair_t<const fastgltf::Accessor*, size_t>> {};
      mesh_accessor_offset_pairs.reserve(
        utility::enum_range<geometry::vertex_component_bit_mask_et>().size() * asset.meshes.size());

      auto mesh_data_byte_count = size_t {0};
      for (auto i = 0uz; i < asset.meshes.size(); ++i) {
        const auto& mesh = asset.meshes[i];

        native_scene.m_meshes.emplace_back(resource_st {a_filepath, mesh.name});
        auto& native_mesh = native_scene.m_meshes.back();
        for (const auto& primitive : mesh.primitives) {
          // determine which components the mash has
          for (const auto component : utility::bit_mask_enum_range<geometry::vertex_component_bit_mask_et>()) {
            const auto native_vertex_component = geometry::vertex_component_bit_mask_et {component};
            const auto component_string = gltf_vertex_component_string(native_vertex_component);
            const auto attribute = primitive.findAttribute(component_string);

            if (attribute == primitive.attributes.cend())
              continue;

            mesh_accessor_offset_pairs.emplace_back(&asset.accessors[attribute->accessorIndex],
                                                    native_mesh.m_host_mesh.m_vertex_model_size);
            native_mesh.m_host_mesh.m_vertex_model_size += geometry::vertex_component_size(native_vertex_component);
            native_mesh.m_host_mesh.m_vertex_component_bit_mask |= component;
          }
        }
        native_mesh.m_host_mesh.m_vertex_count = mesh_accessor_offset_pairs.back().first->count;
        native_mesh.m_host_mesh.m_index_count = asset.accessors[mesh.primitives.front().indicesAccessor.value()].count;

        if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
          native_mesh.m_host_mesh.m_vertex_storage.resize(
            native_mesh.m_host_mesh.m_vertex_model_size * native_mesh.m_host_mesh.m_vertex_count);
          native_mesh.m_host_mesh.m_index_storage.resize(native_mesh.m_host_mesh.m_index_count);

          static_assert(sizeof(typename std::remove_cvref_t<decltype(native_mesh.m_host_mesh)>::vertex_storage_t::value_type)
                        == sizeof(byte_t));
          mesh_data_byte_count += native_mesh.m_host_mesh.m_vertex_storage.size()
                                + native_mesh.m_host_mesh.m_index_storage.size() * index_byte_count;
        }
        native_mesh.m_host_mesh.m_aabb = geometry::aabb_st {.m_min = {0.0, 0.0, 0.0}, .m_max = {0.0, 0.0, 0.0}};
      }
      // generate a virtual suballocation for transfer to device memory
      if (a_configuration.m_mesh_processing.m_propagate_to_device_memory) {
        native_scene.m_memory_mapped_mesh_data
          = a_configuration.m_device_buffer->request_suballocation<byte_t>(mesh_data_byte_count);
      }
      // transfer mesh data
      for (auto i = 0uz, accessor_index = 0uz, device_memory_offset = 0uz; i < asset.meshes.size(); ++i) {
        auto& native_mesh = native_scene.m_meshes[i];
        using namespace geometry;
        using vertex_component_tuple_t
          = tuple_t<vertex_position_st, vertex_normal_st, vertex_tangent_st, vertex_uv_st, vertex_color_st, vertex_joints_st, vertex_weights_st>;
        // write vertex component data
        static_for_loop<0, std::tuple_size_v<vertex_component_tuple_t>>([&a_configuration,
                                                                         &asset,
                                                                         &native_scene,
                                                                         &mesh_accessor_offset_pairs,
                                                                         &native_mesh,
                                                                         &accessor_index,
                                                                         &device_memory_offset]<size_t tp_index> {
          if (native_mesh.m_host_mesh.m_vertex_component_bit_mask bitand utility::set_bit<tp_index + 1>()) {
            using component_t = typename std::tuple_element_t<tp_index, vertex_component_tuple_t>::array_t;
            fastgltf::iterateAccessorWithIndex<component_t>(
              asset,
              *mesh_accessor_offset_pairs[accessor_index].first,
              [&](const auto& a_component, const size_t a_index) {
                const auto data_offset
                  = a_index * native_mesh.m_host_mesh.m_vertex_model_size + mesh_accessor_offset_pairs[accessor_index].second;
                // transfer to host memory
                if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
                  std::memcpy(native_mesh.m_host_mesh.m_vertex_storage.data() + data_offset, &a_component, sizeof(a_component));
                }
                // transfer to device memory
                if (a_configuration.m_mesh_processing.m_propagate_to_device_memory) {
                  std::memcpy(native_scene.m_memory_mapped_mesh_data.data() + data_offset + device_memory_offset,
                              &a_component,
                              sizeof(a_component));
                }
                // generate bounding boxes
                if constexpr (std::is_same_v<std::tuple_element_t<tp_index, vertex_component_tuple_t>, vertex_position_st>) {
                  native_mesh.m_host_mesh.m_aabb.m_min
                    = native_mesh.m_host_mesh.m_aabb.m_min.cwiseMin(position_t<> {a_component.data()});
                  native_mesh.m_host_mesh.m_aabb.m_max
                    = native_mesh.m_host_mesh.m_aabb.m_max.cwiseMax(position_t<> {a_component.data()});
                }
              });
            ++accessor_index;
          }
        });
        // write vertex index data
        if (a_configuration.m_mesh_processing.m_preserve_in_host_memory) {
          fastgltf::copyFromAccessor<index_t>(asset,
                                              asset.accessors[asset.meshes[i].primitives.front().indicesAccessor.value()],
                                              native_mesh.m_host_mesh.m_index_storage.data());
        }
        if (a_configuration.m_mesh_processing.m_propagate_to_device_memory) {
          device_memory_offset += native_mesh.m_host_mesh.m_vertex_count * native_mesh.m_host_mesh.m_vertex_model_size;
          const auto destination = native_scene.m_memory_mapped_mesh_data.data() + device_memory_offset;
          fastgltf::copyFromAccessor<index_t>(asset,
                                              asset.accessors[asset.meshes[i].primitives.front().indicesAccessor.value()],
                                              destination);
          device_memory_offset += native_mesh.m_host_mesh.m_index_count * index_byte_count;
        }
      }

      // iterate scene nodes
      const auto root_node_count = gltf_scene.nodeIndices.size();
      if (root_node_count > 1) {
        warning(std::format("scene loaded from {0} has {1} root nodes", a_filepath.c_str(), root_node_count));
      }

      const auto iterate_node_hierarchy
        = [&asset, &native_scene](this auto&& a_this, const decltype(gltf_scene.nodeIndices)::value_type a_parent_index) -> void {
        for (const auto& child_index : asset.nodes[a_parent_index].children) {
          const auto& child_node = asset.nodes[child_index];
          const auto transformation_matrix = fastgltf::getTransformMatrix(child_node);
          const auto mesh_index = child_node.meshIndex ? child_node.meshIndex.value() : resource_st::null_resource_index;
          native_scene.m_nodes.insert_node(
            scene_st::node_data_st {.m_transformation = *std::bit_cast<geometry::transformation_t<>*>(&transformation_matrix),
                                    .m_resource_index = static_cast<resource_index_t>(mesh_index)},
            a_parent_index);

          // call recursively
          a_this(child_index);
        }
      };

      for (auto i = 0uz; i < root_node_count; ++i) {
        const auto& root_node = asset.nodes[gltf_scene.nodeIndices[i]];
        const auto transformation_matrix = fastgltf::getTransformMatrix(root_node);
        const auto mesh_index = root_node.meshIndex ? root_node.meshIndex.value() : resource_st::null_resource_index;
        native_scene.m_nodes.insert_node(
          scene_st::node_data_st {.m_transformation = *std::bit_cast<geometry::transformation_t<>*>(&transformation_matrix),
                                  .m_resource_index = static_cast<resource_index_t>(mesh_index)},
          gltf_scene.nodeIndices[i]);
        iterate_node_hierarchy(gltf_scene.nodeIndices[i]);
      }
      contract_assert(native_scene.m_nodes.node_count() == asset.nodes.size());

      auto processed_image_data = vector_t<span_t<byte_t>> {};
      auto processed_image_data_byte_count = size_t {0};
      processed_image_data.reserve(asset.images.size());

      // initial image processing
      for (auto& gltf_image : asset.images) {
        constexpr auto requested_channel_count = 4;

        contract_assert(std::holds_alternative<fastgltf::sources::BufferView>(gltf_image.data));
        auto& gltf_buffer_view = asset.bufferViews[std::get<fastgltf::sources::BufferView>(gltf_image.data).bufferViewIndex];

        auto& gltf_buffer = asset.buffers[gltf_buffer_view.bufferIndex].data;
        contract_assert(std::holds_alternative<fastgltf::sources::Array>(gltf_buffer));

        auto& gltf_byte_array = std::get<fastgltf::sources::Array>(gltf_buffer).bytes;
        auto crude_image_data
          = span_t<byte_t> {gltf_byte_array.begin() + gltf_buffer_view.byteOffset, gltf_buffer_view.byteLength};
        auto native_image = image_st {resource_st {a_filepath, gltf_image.name}};
        native_image.m_host_image.m_data_byte_count = crude_image_data.size_bytes();

        const auto processed_image_bytes = std::bit_cast<obs_ptr_t<byte_t>>(
          stbi_load_from_memory(std::bit_cast<const stbi_uc*>(crude_image_data.data()),
                                crude_image_data.size(),
                                std::bit_cast<int*>(&native_image.m_host_image.m_width),
                                std::bit_cast<int*>(&native_image.m_host_image.m_height),
                                std::bit_cast<int*>(&native_image.m_host_image.m_channel_count),
                                requested_channel_count));

        contract_assert(processed_image_bytes);
        processed_image_data.emplace_back(processed_image_bytes, crude_image_data.size_bytes());
        processed_image_data_byte_count += crude_image_data.size_bytes();

        native_scene.m_images.emplace_back(std::move(native_image));
      }
      // generate a virtual suballocation for transfer to device memory
      if (a_configuration.m_image_processing.m_propagate_to_device_memory) {
        native_scene.m_memory_mapped_image_data
          = a_configuration.m_device_buffer->request_suballocation<byte_t>(processed_image_data_byte_count);
      }
      // transfer image data
      for (auto i = 0uz, device_memory_offset = 0uz; i < native_scene.m_images.size(); ++i) {
        // transfer to host memory
        if (a_configuration.m_image_processing.m_preserve_in_host_memory) {
          native_scene.m_images[i].m_host_image.m_data.resize(native_scene.m_images[i].m_host_image.m_data_byte_count);
          std::memcpy(native_scene.m_images[i].m_host_image.m_data.data(),
                      processed_image_data[i].data(),
                      processed_image_data[i].size_bytes());
        }
        // transfer to device memory
        if (a_configuration.m_image_processing.m_propagate_to_device_memory) {
          std::memcpy(native_scene.m_memory_mapped_image_data.data() + device_memory_offset,
                      processed_image_data[i].data(),
                      processed_image_data[i].size_bytes());
          device_memory_offset += processed_image_data[i].size_bytes();
        }
        stbi_image_free(processed_image_data[i].data());
      }

      input_subsystem_st::filesystem_st::scene_registry.emplace_back(std::move(native_scene));
    }
    auto read_shader_file(const file_path_t& a_filepath,
                          const input_subsystem_st::filesystem_st::shader_processing_configuration_st& a_configuration) -> void {
      const auto shader_source = read_text_file(a_filepath);

      const auto& slang_session = input_subsystem_st::filesystem_st::slang_session;
      auto slang_diagnostics = Slang::ComPtr<slang::IBlob> {};

      const auto module
        = Slang::ComPtr<slang::IModule> {slang_session->loadModuleFromSourceString(a_filepath.filename().c_str(),
                                                                                   a_filepath.c_str(),
                                                                                   shader_source.c_str(),
                                                                                   slang_diagnostics.writeRef())};
      process_slang_diagnostics(slang_diagnostics);
      contract_assert(module);
      auto shader = shader_st {resource_st {a_filepath}};

      const auto entry_point_count = static_cast<size_t>(module->getDefinedEntryPointCount());
      shader.m_compiled_spir_v.resize(entry_point_count);
      shader.m_shader_source.m_source_code = std::move(shader_source);
      auto component_types = vector_t<obs_ptr_t<slang::IComponentType>> {};
      component_types.reserve(entry_point_count + 1);
      component_types.emplace_back(module);

      // discover and compose program entry points
      for (auto i = 0uz; i < entry_point_count; ++i) {
        auto entry_point = Slang::ComPtr<slang::IEntryPoint> {};
        const auto entry_point_discovery_result = module->getDefinedEntryPoint(i, entry_point.writeRef());
        contract_assert(SLANG_SUCCEEDED(entry_point_discovery_result));
        component_types.emplace_back(entry_point);
      }
      // link program
      auto composed_program = Slang::ComPtr<slang::IComponentType> {};
      const auto composite_component_type_creation_result
        = slang_session->createCompositeComponentType(component_types.data(),
                                                      component_types.size(),
                                                      composed_program.writeRef(),
                                                      slang_diagnostics.writeRef());
      process_slang_diagnostics(slang_diagnostics);
      contract_assert(SLANG_SUCCEEDED(composite_component_type_creation_result));

      auto linked_program = Slang::ComPtr<slang::IComponentType> {};
      const auto linking_result = composed_program->link(linked_program.writeRef(), slang_diagnostics.writeRef());
      process_slang_diagnostics(slang_diagnostics);
      contract_assert(SLANG_SUCCEEDED(linking_result));
      // generate reflection data
      const auto program_layout = linked_program->getLayout(0, slang_diagnostics.writeRef());
      process_slang_diagnostics(slang_diagnostics);
      if (a_configuration.m_cache_reflection_data_to_filesystem) {
        auto program_layout_binary_json = Slang::ComPtr<slang::IBlob> {};
        program_layout->toJson(program_layout_binary_json.writeRef());

        auto program_layout_string_json = std::string {static_cast<const char_t*>(program_layout_binary_json->getBufferPointer()),
                                                       program_layout_binary_json->getBufferSize()};
        auto json_file_path = file_path_t {a_filepath}.replace_extension(".json");
        output::write_text_file(json_file_path, program_layout_string_json);
      }

      // generate spir_v bytecode
      for (auto i = 0uz; i < entry_point_count; ++i) {
        auto spir_v_bytecode = Slang::ComPtr<slang::IBlob> {};
        const auto compilation_result = linked_program->getEntryPointCode(i,
                                                                          /*target_index*/ 0,
                                                                          spir_v_bytecode.writeRef(),
                                                                          slang_diagnostics.writeRef());
        process_slang_diagnostics(slang_diagnostics);
        contract_assert(SLANG_SUCCEEDED(compilation_result));

        // process metadata
        auto entry_point_metadata = Slang::ComPtr<slang::IMetadata> {};
        const auto metadata_acquisition_result
          = linked_program->getEntryPointMetadata(i, 0, entry_point_metadata.writeRef(), slang_diagnostics.writeRef());
        process_slang_diagnostics(slang_diagnostics);
        contract_assert(SLANG_SUCCEEDED(metadata_acquisition_result));

        const auto spir_v_bytecode_count = spir_v_bytecode->getBufferSize();
        shader.m_compiled_spir_v[i].m_data.resize(spir_v_bytecode_count);
        std::memcpy(shader.m_compiled_spir_v[i].m_data.data(),
                    spir_v_bytecode->getBufferPointer(),
                    spir_v_bytecode->getBufferSize());
      }
      // generate the shader pipeline
      if (a_configuration.m_device_context.has_value()) {
        auto shader_pipeline_configuration = graphics::vulkan::shader_object_pipeline_ct<>::configuration_st {};
        shader_pipeline_configuration.m_shader_create_info.reserve(entry_point_count);

        const auto shader_pipeline_flags = vk::ShaderCreateFlagsEXT {
          entry_point_count > 1 ? vk::ShaderCreateFlagBitsEXT::eLinkStage : vk::ShaderCreateFlagBitsEXT {}};
        for (auto i = 0uz; i < entry_point_count; ++i) {
          const auto entry_point_reflection = program_layout->getEntryPointByIndex(i);
          const auto entry_point_name = entry_point_reflection->getName();
          const auto entry_point_stage = vulkan_shader_stage(entry_point_reflection->getStage());
          const auto next_entry_point_stage = entry_point_stage == vk::ShaderStageFlagBits::eVertex
                                              ? vk::ShaderStageFlagBits::eFragment
                                              : vk::ShaderStageFlagBits {};
          const auto descriptor_set_layout = static_cast<vk::DescriptorSetLayout>(
            a_configuration.m_device_context.value().m_pipeline_layout->combined_image_sampler_layout());

          shader_pipeline_configuration.m_shader_create_info.emplace_back(vk::ShaderCreateInfoEXT {
            shader_pipeline_flags, entry_point_stage, next_entry_point_stage, vk::ShaderCodeTypeEXT::eSpirv,
            shader.m_compiled_spir_v[i].m_data.size(), shader.m_compiled_spir_v[i].m_data.data(), entry_point_name,
            /*set_layout_count*/ 1, &descriptor_set_layout, /*push_constant_range_count*/ 1,
            &a_configuration.m_device_context.value().m_pipeline_layout->push_constant_range()});
        }

        shader.m_shader_pipeline.emplace(*a_configuration.m_device_context.value().m_logical_device,
                                         *a_configuration.m_device_context.value().m_physical_device,
                                         shader_pipeline_configuration);
      }

      input_subsystem_st::filesystem_st::shader_registry.emplace_back(std::move(shader));
    }
  }
}