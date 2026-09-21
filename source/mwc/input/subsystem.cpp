#include "mwc/input/subsystem.hpp"
#include "mwc/graphics/vulkan/shader_object.hpp"
#include "mwc/output/output.hpp"
#include "mwc/graphics/camera/camera.hpp"
#include "mwc/core/diagnostic/log/logging.hpp"

import mwc_file_subsystem;
import mwc_directory;
import mwc_breakpoint;
import mwc_vertex_model;
import mwc_stb_image;
import mwc_usd;

import vulkan;

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
      mouse_st::previous_cursor_position = mouse_st::cursor_positon_st {{0.0}, {0.0}};
      mouse_st::current_cursor_position = mouse_st::cursor_positon_st {{0.0}, {0.0}};

      // filesystem_st::gltf_parser = fastgltf::Parser {fastgltf::Extensions::None};

      // slang configuration
      const auto slang_global_session_generation_result
        = slang::createGlobalSession(filesystem_st::slang_global_session.writeRef());
      contract_assert(slang_succeeded(slang_global_session_generation_result));
      const auto& shader_data_directory_path = filesystem::directory(filesystem::directory_et::e_shader).string().c_str();
      const auto slang_target_description
        = slang::TargetDesc {.format = SLANG_SPIRV, .profile = filesystem_st::slang_global_session->findProfile("spirv_1_6")};
      auto slang_compiler_options = array_t<slang::CompilerOptionEntry, 2> {
        slang::CompilerOptionEntry {slang::CompilerOptionName::EmitSpirvDirectly,
                                    slang::CompilerOptionValue {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}},
        {slang::CompilerOptionName::VulkanUseEntryPointName,
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
      contract_assert(slang_succeeded(slang_session_generation_result));

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
      // load the scene
      const auto stage_ptr = usd::UsdStage::Open(a_filepath);
      contract_assert(stage_ptr);

      // native scene
      auto native_scene = scene_st {resource_st {a_filepath, a_filepath.filename().string()}};

      // parse meshes
      auto unique_mesh_reference_map = unordered_multimap_t<usd::UsdGeomMesh, usd::UsdPrim> {}
      for (const auto& primitive : stage_ptr->Traverse()) {
        if (primitive.IsA<usd::UsdGeomMesh>()) {
          const auto mesh = usd::UsdGeomMesh {primitive};
          const auto primvars_api = usd::UsdGeomPrimvarsAPI {mesh};
          const auto shade_material_binding_api = pxr::UsdShadeMaterialBindingAPI {mesh};

          const auto usd_indices = pxr::VtArray<sint32_t> {};
          const auto indices_extraction_result = mesh.GetFaceVertexIndicesAttr().Get(&usd_indices);

          auto usd_vertices = usd::VtArray<usd::GfVec3f> {};
          const auto vertex_extraction_result = mesh.GetPointsAttr().Get(&usd_vertices);
          contract_assert(vertex_extraction_result);

          auto native_mesh = mesh_st {resource_st {a_filepath, primitive.GetName().GetString()}};
          native_mesh.m_host_mesh.m_vertex_storage.emplace_back();
        }
      }
    }
    auto read_shader_file(const file_path_t& a_filepath,
                          const input_subsystem_st::filesystem_st::shader_processing_configuration_st& a_configuration) -> void {
      const auto shader_source = read_text_file(a_filepath);

      const auto& slang_session = input_subsystem_st::filesystem_st::slang_session;
      auto slang_diagnostics = Slang::ComPtr<slang::IBlob> {};

      const auto module
        = Slang::ComPtr<slang::IModule> {slang_session->loadModuleFromSourceString(a_filepath.filename().string().c_str(),
                                                                                   a_filepath.string().c_str(),
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
        contract_assert(slang_succeeded(entry_point_discovery_result));
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
      contract_assert(slang_succeeded(composite_component_type_creation_result));

      auto linked_program = Slang::ComPtr<slang::IComponentType> {};
      const auto linking_result = composed_program->link(linked_program.writeRef(), slang_diagnostics.writeRef());
      process_slang_diagnostics(slang_diagnostics);
      contract_assert(slang_succeeded(linking_result));
      // generate reflection data
      const auto program_layout = linked_program->getLayout(0, slang_diagnostics.writeRef());
      process_slang_diagnostics(slang_diagnostics);
      if (a_configuration.m_cache_reflection_data_to_filesystem) {
        auto program_layout_binary_json = Slang::ComPtr<slang::IBlob> {};
        program_layout->toJson(program_layout_binary_json.writeRef());

        auto program_layout_string_json = std::string {static_cast<const char_t*>(program_layout_binary_json->getBufferPointer()),
                                                       program_layout_binary_json->getBufferSize()};
        auto json_file_path = file_path_t {a_filepath}.replace_extension(".json");
        output::write_file(json_file_path, program_layout_string_json);
      }

      // generate spir_v bytecode
      for (auto i = 0uz; i < entry_point_count; ++i) {
        auto spir_v_bytecode = Slang::ComPtr<slang::IBlob> {};
        const auto compilation_result = linked_program->getEntryPointCode(i,
                                                                          /*target_index*/ 0,
                                                                          spir_v_bytecode.writeRef(),
                                                                          slang_diagnostics.writeRef());
        process_slang_diagnostics(slang_diagnostics);
        contract_assert(slang_succeeded(compilation_result));

        // process metadata
        auto entry_point_metadata = Slang::ComPtr<slang::IMetadata> {};
        const auto metadata_acquisition_result
          = linked_program->getEntryPointMetadata(i, 0, entry_point_metadata.writeRef(), slang_diagnostics.writeRef());
        process_slang_diagnostics(slang_diagnostics);
        contract_assert(slang_succeeded(metadata_acquisition_result));

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