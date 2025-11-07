#include "mwc/graphics/graphics.hpp"
#include "mwc/graphics/vulkan/debug.hpp"
#include "mwc/input/subsystem.hpp"

//#include <eigen3/Eigen/src/Geometry/Transform.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

//#include <eigen3/Eigen/src/Geometry/AngleAxis.h>

//#include <eigen3/Eigen/src/Geometry/Transform.h>

import mwc_memory_conversion;

namespace mwc {
  namespace graphics {
    using float3 = math::vector_t<float32_t, 3>;
    auto look_at(const float3 m_center, const float3 m_eye) -> math::matrix_t<float32_t, 4, 4> {
      const float3 up = {0.0f, 1.0f, 0.0f};
      float3 forward = (m_center - m_eye).normalized();
      float3 right = forward.cross(up.normalized()).normalized();

      math::matrix_t<float32_t, 4, 4> viewMatrix = math::matrix_t<float32_t, 4, 4>::Identity();
      viewMatrix.row(0).head<3>() = right.transpose();
      viewMatrix.row(1).head<3>() = up.transpose();
      viewMatrix.row(2).head<3>() = -forward.transpose(); // Negative forward for right-handed coordinate systems

      viewMatrix.row(0)(3) = -right.dot(m_eye);
      viewMatrix.row(1)(3) = -up.dot(m_eye);
      viewMatrix.row(2)(3) = forward.dot(m_eye); // Note: positive here due to negative forward in rotation

      return {viewMatrix};
    }

    auto graphics_ct::record_mesh_data_transfer_to_device_memory(input::scene_st& a_scene,
                                                                 const vk::raii::CommandBuffer& a_command_buffer) -> void {
      // generate a new vertex buffer
      m_vertex_buffers.emplace_back(
        m_logical_device, m_memory_allocator,
        vulkan::buffer_ct::configuration_st {
        .m_buffer_create_info
        = vk::BufferCreateInfo {vk::BufferCreateFlags {}, a_scene.m_memory_mapped_mesh_data.size(),
                                vk::BufferUsageFlagBits::eVertexBuffer bitor vk::BufferUsageFlagBits::eIndexBuffer
                                  bitor vk::BufferUsageFlagBits::eTransferDst bitor vk::BufferUsageFlagBits::eShaderDeviceAddress},
        .m_allocation_create_info = vma::AllocationCreateInfo {vma::AllocationCreateFlags {}, vma::MemoryUsage::eAuto}});
      m_vertex_buffers.back().debug_name("vertex_buffer");

      // record transfer to device memory
      const auto suballocation_offset = vk::DeviceSize {pointer_cast(a_scene.m_memory_mapped_mesh_data.data())
                                                        - pointer_cast(m_common_buffer.mapped_data_pointer())};
      const auto copy_information
        = vk::BufferCopy2 {/*source_offset*/ suballocation_offset,
                           /*destination_offset*/ vk::DeviceSize {0}, a_scene.m_memory_mapped_mesh_data.size()};
      a_command_buffer.copyBuffer2(
        vk::CopyBufferInfo2 {m_common_buffer.native_handle(), m_vertex_buffers.back().native_handle(), copy_information});

      // record mesh device addresses
      for (auto offset = vk::DeviceSize {0}; auto& mesh : a_scene.m_meshes) {
        mesh.m_device_mesh.m_vertex_buffer.m_buffer = m_vertex_buffers.back().native_handle();
        mesh.m_device_mesh.m_vertex_buffer.m_offset = offset;
        mesh.m_device_mesh.m_vertex_buffer.m_size = mesh.m_host_mesh.m_vertex_storage.size();
        offset += mesh.m_host_mesh.m_vertex_storage.size();

        mesh.m_device_mesh.m_index_buffer.m_buffer = m_vertex_buffers.back().native_handle();
        mesh.m_device_mesh.m_index_buffer.m_offset = offset;
        mesh.m_device_mesh.m_index_buffer.m_size = mesh.m_host_mesh.m_index_storage.size() * sizeof(uint32_t);
        offset += mesh.m_host_mesh.m_index_storage.size() * sizeof(uint32_t);
      }
    }

    graphics_ct::graphics_ct(const window_ct& a_window, const semantic_version_st& a_engine_version,
                             const configuration_st& a_configuration)
    : m_window {a_window},
      m_context {vulkan::context_st::configuration_st {.m_engine_version = a_engine_version}},
      m_instance {m_context},
      m_physical_device {m_instance},
      m_surface {m_window, m_instance, m_physical_device},
      m_queue_families {m_physical_device, m_surface},
      m_logical_device {m_physical_device, m_queue_families},
      m_memory_allocator {m_context, m_instance, m_physical_device, m_logical_device},
      m_swapchain {m_physical_device, m_surface, m_queue_families, m_logical_device, m_memory_allocator},
      m_pipeline_layout {m_physical_device, m_logical_device},
      m_command_pool {m_logical_device, m_queue_families},
      m_frame_synchronizer {m_logical_device, m_command_pool,
                            decltype(m_frame_synchronizer)::configuration_st {.m_frame_count = 4}},
      m_graphics_queue {m_logical_device,
                        vulkan::queue_ct::configuration_st {.m_family_index = m_queue_families.graphics().m_index}},
      m_transfer_queue {m_logical_device,
                        vulkan::queue_ct::configuration_st {.m_family_index = m_queue_families.transfer().m_index}},
      m_common_buffer {
      m_logical_device, m_memory_allocator,
      vulkan::suballocated_memory_mapped_buffer_ct::
        configuration_st {
        .m_memory_mapped_buffer_configuration
        = vulkan::memory_mapped_buffer_ct::configuration_st {.m_buffer_create_info
                                                             = vk::BufferCreateInfo {vk::BufferCreateFlags {},
                                                                                     vk::DeviceSize {byte_count<std::mega>(256)},
                                                                                     vk::BufferUsageFlagBits::eTransferSrc,
                                                                                     vk::SharingMode::eExclusive,
                                                                                     /*pQueueFamilyIndices*/ 0},
                                                             .m_allocation_create_info
                                                             = vma::AllocationCreateInfo {vma::AllocationCreateFlags {
                                                                                          vma::AllocationCreateFlagBits::eMapped
                                                                                          bitor vma::AllocationCreateFlagBits::eHostAccessRandom},
                                                                                          vma::MemoryUsage::eAuto,
                                                                                          vk::MemoryPropertyFlags {
                                                                                          vk::MemoryPropertyFlagBits::eHostVisible},
                                                                                          vk::MemoryPropertyFlags {
                                                                                          vk::MemoryPropertyFlagBits::eDeviceLocal}}},
        .m_virtual_allocator_configuration
        = virtual_allocator_ct::configuration_st {.m_virtual_block_create_flags = vma::VirtualBlockCreateFlags {}}}},
      m_dynamic_rendering_state {m_surface},
      m_vertex_buffers {},
      m_user_interface {dear_imgui_ct {a_window, m_context, m_instance, m_physical_device, m_logical_device,
                                       m_queue_families.graphics(), m_graphics_queue, m_swapchain}},
      m_camera {camera_ct::configuration_st<camera_projection_et::e_perspective>::default_configuration()},
      m_configuration {a_configuration} {
      auto scene_cfg = input::input_subsystem_st::filesystem_st::scene_processing_configuration_st::default_configuration();
      scene_cfg.m_device_buffer = &m_common_buffer;

      input::read_scene_file("/home/billy/dev/mwc/data/mesh/Untitled.glb",
                             {{false, true, true}, {false, true, true}, &m_common_buffer});
      contract_assert(input::input_subsystem_st::filesystem_st::scene_registry.size() > 0);

      auto& frame_data = m_frame_synchronizer.m_synchronization_data[m_frame_synchronizer.m_frame_index];
      const auto& cmd = frame_data.m_command_buffer;

      cmd.begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
      record_mesh_data_transfer_to_device_memory(input::input_subsystem_st::filesystem_st::scene_registry[0], cmd);
      cmd.end();
      vk::CommandBufferSubmitInfo submit_info {*cmd};
      m_graphics_queue->submit2(vk::SubmitInfo2 {vk::SubmitFlags {}, {}, submit_info, {}});
      m_graphics_queue->waitIdle();
      m_common_buffer.release_suballocation(input::input_subsystem_st::filesystem_st::scene_registry[0].m_memory_mapped_mesh_data);

      auto shader_cfg = input::input_subsystem_st::filesystem_st::shader_processing_configuration_st {
        .m_device_context = {{&m_logical_device, &m_physical_device, &m_pipeline_layout}},
        true,
        true,
        true};
      mwc::file_path_t shader_path = mwc::filesystem::directory(mwc::filesystem::directory_et::e_shader);
      shader_path /= "default.slang";
      mwc::input::read_shader_file(shader_path, shader_cfg);
    }

    auto graphics_ct::render() -> void {
      //camera
      const auto cursor_position_delta_x = float(input::input_subsystem_st::mouse_st::current_cursor_position.m_x
                                                 - input::input_subsystem_st::mouse_st::previous_cursor_position.m_x);
      const auto cursor_position_delta_y = float(input::input_subsystem_st::mouse_st::current_cursor_position.m_y
                                                 - input::input_subsystem_st::mouse_st::previous_cursor_position.m_y);

      auto camera_trans_comp = m_camera.components<ecs::transformation_st, ecs::camera_projection_st, ecs::camera_type_st>();
      auto& camera_trans = std::get<0>(camera_trans_comp).m_transformation;
      const auto& camera_proj = std::get<1>(camera_trans_comp).m_projection;
      auto cam_quat_x = math::quaternion_t<float32_t> {
        math::angle_axis_t<float32_t> {geometry::radians(cursor_position_delta_y), math::vector_t<float32_t, 3>::UnitX()}};
      auto cam_quat_y = math::quaternion_t<float32_t> {
        math::angle_axis_t<float32_t> {geometry::radians(cursor_position_delta_x), math::vector_t<float32_t, 3>::UnitY()}};
      auto camera_quat = math::quaternion_t<float32_t> {camera_trans.rotation()};

      math::vector_t<float32_t, 3> cam_pos = camera_trans.translation();
      float3 cam_look_center = {0.0f, 0.0f, 0.0f};
      math::quaternion_t<float32_t> full_rot = cam_quat_y * camera_quat * cam_quat_x;

      const auto cam_view_dir
        = math::vector_t<float32_t, 3> {camera_trans(0, 2), camera_trans(1, 2), camera_trans(2, 2) /*norm_axis*/};
      constexpr auto speed = 0.01f;
      const auto cam_right_dir = cam_view_dir.cross(math::vector_t<float32_t, 3> {0.0f, 1.0f, 0.0f});
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eW)) {
        cam_pos += cam_view_dir * -speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eS)) {
        cam_pos += cam_view_dir * speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eD)) {
        cam_pos += cam_right_dir * speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eA)) {
        cam_pos += cam_right_dir * -speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eSpace)) {
        cam_pos += float3 {0.0f, 1.0f, 0.0f} * speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eC)) {
        cam_pos += float3 {0.0f, 1.0f, 0.0f} * -speed;
      }
      camera_trans.fromPositionOrientationScale(cam_pos, full_rot, math::vector_t<float32_t, 3> {1.0f, 1.0f, 1.0f});

      //m_graphics_queue.command_pool()->reset(vk::CommandPoolResetFlagBits {});
      auto& frame_data = m_frame_synchronizer.m_synchronization_data[m_frame_synchronizer.m_frame_index];

      const auto& cmd = frame_data.m_command_buffer;
      cmd.begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

      const auto [result, image_idx, /*image_acquired_semaphore,*/ render_info]
        = m_swapchain.acquire_next_image(cmd, *frame_data.m_image_acquired_semaphore);

      // begin rendering
      cmd.beginRendering(render_info);
      // dynamic rendring state
      m_dynamic_rendering_state.bind(cmd);

      const auto inv_camera = camera_trans.inverse();
      // push constants
      auto push_constants = vulkan::push_constant_st {};
      push_constants.m_model = camera_proj.matrix() * inv_camera.matrix() /* * math::matrix_t<float32_t, 4, 4>::Ones()*/;
      push_constants.m_registers.m_register_0 = m_vertex_buffers[0].address();
      push_constants.m_view_data.m_view_direction
        = math::vector_t<float32_t, 4> {cam_view_dir[0], cam_view_dir[1], cam_view_dir[2], 1.0f};
      push_constants.m_view_data.m_view_position = math::vector_t<float32_t, 4> {cam_pos[0], cam_pos[1], cam_pos[2], 0.0f};
      cmd.pushConstants<vulkan::push_constant_st>(m_pipeline_layout.native_handle(), vk::ShaderStageFlagBits::eAll, 0,
                                                  push_constants);

      // shader
      const auto shader_stages
        = array_t<vk::ShaderStageFlagBits, 2> {vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment};
      const auto& shader = input::input_subsystem_st::filesystem_st::shader_registry.front().m_shader_pipeline.value();
      cmd.bindShadersEXT(shader_stages, shader.shader_objects());
      const auto inactive_shader_stages = array_t<vk::ShaderStageFlagBits, 3> {
        vk::ShaderStageFlagBits::eGeometry,
        vk::ShaderStageFlagBits::eTessellationEvaluation,
        vk::ShaderStageFlagBits::eTessellationControl,
      };
      cmd.bindShadersEXT(inactive_shader_stages, {nullptr, nullptr, nullptr});

      const auto& mesh = input::input_subsystem_st::filesystem_st::scene_registry[0].m_meshes[0];
      // vertex buffer
      auto vert_attribs = array_t<vk::VertexInputAttributeDescription2EXT, 4> {};
      vert_attribs[0] = {0, 0, vk::Format::eR32G32B32Sfloat, 0};
      vert_attribs[1] = {1, 0, vk::Format::eR32G32B32Sfloat, 12};
      vert_attribs[2] = {2, 0, vk::Format::eR32G32B32A32Sfloat, 24};
      vert_attribs[3] = {3, 0, vk::Format::eR32G32Sfloat, 40};
      cmd.bindVertexBuffers(0, mesh.m_device_mesh.m_vertex_buffer.m_buffer, {0});
      cmd.setVertexInputEXT(vk::VertexInputBindingDescription2EXT {0, uint32_t(mesh.m_host_mesh.m_vertex_model_size),
                                                                   vk::VertexInputRate::eVertex, 1},
                            vert_attribs);
      // index buffer
      cmd.bindIndexBuffer(mesh.m_device_mesh.m_index_buffer.m_buffer, mesh.m_device_mesh.m_index_buffer.m_offset,
                          vk::IndexType::eUint32);
      cmd.drawIndexed(mesh.m_device_mesh.m_index_buffer.m_size / sizeof(uint32_t), 1, 0, 0, 0);
      // end rendering
      //std::cout << "frame idx: " << m_frame_synchronizer.m_frame_index << '\n';

      // user interface
      //imgui
      m_user_interface.begin_frame();
      m_user_interface.generate_debug_interface();
      m_user_interface.render(cmd);

      cmd.endRendering();
      m_swapchain.transition_layout<vulkan::swapchain_ct::layout_state_et::e_presentation>(cmd);
      cmd.end();

      auto wait_semaphores
        = vk::SemaphoreSubmitInfo {*frame_data.m_image_acquired_semaphore, 0, vk::PipelineStageFlagBits2::eTopOfPipe};
      vk::CommandBufferSubmitInfo submit_info {*cmd};
      auto signal_semaphores
        = vk::SemaphoreSubmitInfo {*frame_data.m_render_complete_semaphore, 0, vk::PipelineStageFlagBits2::eBottomOfPipe};
      m_graphics_queue->submit2(vk::SubmitInfo2 {vk::SubmitFlags {}, wait_semaphores, submit_info, signal_semaphores});

      //m_logical_device->waitForFences(*render_fence, true, 0);

      //m_logical_device->resetFences(*render_fence);
      auto present_info = vk::PresentInfoKHR {*frame_data.m_render_complete_semaphore, m_swapchain.native_handle(), image_idx};
      m_graphics_queue->presentKHR(present_info);
      //m_logical_device->waitIdle();
      if (m_frame_synchronizer.m_frame_count > 0
          and m_frame_synchronizer.m_frame_index % m_frame_synchronizer.m_frame_count == 0) {
        m_graphics_queue->waitIdle();
        m_command_pool->reset(vk::CommandPoolResetFlagBits {});
      }

      m_frame_synchronizer.m_frame_count++;
      m_frame_synchronizer.m_frame_index = (m_frame_synchronizer.m_frame_index + 1) % m_frame_synchronizer.frame_count();
    }
  }
}