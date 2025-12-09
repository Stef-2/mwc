#include "mwc/graphics/graphics.hpp"
#include "mwc/graphics/vulkan/debug.hpp"
#include "mwc/input/subsystem.hpp"

//#include <eigen3/Eigen/src/Geometry/Transform.h>

//#include <eigen3/Eigen/src/Geometry/Transform.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

//#include <eigen3/Eigen/src/Geometry/AngleAxis.h>

//#include <eigen3/Eigen/src/Geometry/Transform.h>

import mwc_memory_conversion;

namespace mwc {
  namespace graphics {
    using float3 = math::vector_t<float32_t, 3>;
    using float4 = math::vector_t<float32_t, 4>;
    using quat = math::quaternion_t<float32_t>;
    using mat3 = math::matrix_t<float32_t, 3, 3>;
    using mat4 = math::matrix_t<float, 4, 4>;

    auto transition_image_layout(const vk::raii::CommandBuffer& a_command_buffer, const vk::Image a_image,
                                 const vk::ImageLayout a_previous_layout, const vk::ImageLayout a_layout) -> void {
      const auto barrier
        = vk::ImageMemoryBarrier2 {vk::PipelineStageFlagBits2::eTransfer,
                                   vk::AccessFlagBits2::eTransferWrite,
                                   vk::PipelineStageFlagBits2::eTransfer,
                                   vk::AccessFlagBits2::eTransferRead,
                                   a_previous_layout,
                                   a_layout,
                                   vk::QueueFamilyIgnored,
                                   vk::QueueFamilyIgnored,
                                   a_image,
                                   {vk::ImageAspectFlagBits::eColor, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers}};

      const auto dependency_info = vk::DependencyInfo {{}, {}, {}, barrier};

      a_command_buffer.pipelineBarrier2(dependency_info);
    }

    auto graphics_ct::record_scene_data_transfer_to_device_memory(input::scene_st& a_scene,
                                                                  const vk::raii::CommandBuffer& a_command_buffer) -> void {
      // mesh data
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
      const auto mesh_data_suballocation_offset = vk::DeviceSize {pointer_cast(a_scene.m_memory_mapped_mesh_data.data())
                                                                  - pointer_cast(m_common_buffer.mapped_data_pointer())};
      const auto copy_information
        = vk::BufferCopy2 {/*source_offset*/ mesh_data_suballocation_offset,
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
      // images
      const auto image_data_suballocation_offset = vk::DeviceSize {pointer_cast(a_scene.m_memory_mapped_image_data.data())
                                                                   - pointer_cast(m_common_buffer.mapped_data_pointer())};
      auto descriptor_write_data = vk::WriteDescriptorSet {};
      auto descriptor_image_infos = vector_t<vk::DescriptorImageInfo> {};
      //descriptor_write_data.reserve(a_scene.m_images.size());
      descriptor_image_infos.resize(a_scene.m_images.size());

      // generate device image resources
      for (auto accumulated_offset = vk::DeviceSize {0}, i = 0uz; auto& image : a_scene.m_images) {
        const auto image_create_info
          = vk::ImageCreateInfo {vk::ImageCreateFlags {},
                                 vk::ImageType::e2D,
                                 vk::Format::eR8G8B8A8Srgb,
                                 vk::Extent3D {image.m_host_image.m_width, image.m_host_image.m_height, 1},
                                 /*mip_levels*/ 1,
                                 /*array_layers*/ 1,
                                 vk::SampleCountFlagBits::e1,
                                 vk::ImageTiling::eOptimal,
                                 vk::ImageUsageFlagBits::eTransferDst bitor vk::ImageUsageFlagBits::eSampled,
                                 vk::SharingMode::eExclusive,
                                 /*queue_family_indices*/ {},
                                 vk::ImageLayout::eUndefined};
        const auto allocation_info = vma::AllocationCreateInfo {vma::AllocationCreateFlags {}, vma::MemoryUsage::eAuto};
        image.m_device_image.m_image
          = vulkan::image_ct {m_logical_device,
                              m_memory_allocator,
                              {.m_image_create_info = image_create_info, .m_allocation_create_info = allocation_info}};
        image.m_device_image.m_image.record_layout_transition(
          a_command_buffer, vulkan::image_ct::layout_transition_configuration_st::default_configuration());

        image.m_device_image.m_image.debug_name(image.m_name);
        image.m_device_image.m_image_view
          = m_logical_device
              ->createImageView(vk::ImageViewCreateInfo {
              vk::ImageViewCreateFlags {}, image.m_device_image.m_image.native_handle(), vk::ImageViewType::e2D,
              vk::Format::eR8G8B8A8Srgb, vk::ComponentMapping {},
              vk::ImageSubresourceRange {vk::ImageAspectFlagBits::eColor, /*base_mip_level*/ 0, vk::RemainingMipLevels,
                                         /*base_array_layer*/ 0, vk::RemainingArrayLayers}})
              .value;
        image.m_device_image.m_sampler = m_logical_device->createSampler(vk::SamplerCreateInfo {}).value;

        const auto buffer_image_copy
          = vk::BufferImageCopy2 {image_data_suballocation_offset + accumulated_offset,
                                  /*image.m_host_image.m_width*/ 0,
                                  /*image.m_host_image.m_height*/ 0,
                                  vk::ImageSubresourceLayers {vk::ImageAspectFlagBits::eColor, /*mip_level*/ 0,
                                                              /*base_array_layer*/ 0, /*layer_count*/ 1},
                                  vk::Offset3D {},
                                  vk::Extent3D {image.m_host_image.m_width, image.m_host_image.m_height, 1}};
        const auto copy_buffer_to_image_info
          = vk::CopyBufferToImageInfo2 {m_common_buffer.native_handle(), image.m_device_image.m_image.native_handle(),
                                        vk::ImageLayout::eTransferDstOptimal, buffer_image_copy};
        a_command_buffer.copyBufferToImage2(copy_buffer_to_image_info);
        image.m_device_image.m_image.record_layout_transition(
          a_command_buffer, {.m_source_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer,
                             .m_destination_pipeline_stage = vk::PipelineStageFlagBits2::eTransfer,
                             .m_source_access_flags = vk::AccessFlagBits2::eTransferWrite,
                             .m_destination_access_flags = vk::AccessFlagBits2::eTransferRead,
                             .m_old_layout = vk::ImageLayout::eTransferDstOptimal,
                             .m_new_layout = vk::ImageLayout::eShaderReadOnlyOptimal,
                             .m_source_queue_family = vk::QueueFamilyIgnored,
                             .m_destination_queue_family = vk::QueueFamilyIgnored,
                             .m_subresource_range = vk::ImageSubresourceRange {
                             vk::ImageAspectFlagBits::eColor, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers}});

        descriptor_image_infos[i] = vk::DescriptorImageInfo {*image.m_device_image.m_sampler, *image.m_device_image.m_image_view,
                                                             vk::ImageLayout::eShaderReadOnlyOptimal};
        /*descriptor_write_data.emplace_back(vk::WriteDescriptorSet {
                  *m_pipeline_layout.combined_image_sampler_set(),
                  0,
                  static_cast<uint32_t>(i),
                  vk::DescriptorType::eCombinedImageSampler,
                });*/
        image.m_device_image.m_image_index = m_pipeline_layout.acquire_descriptor_index();

        accumulated_offset += image.m_host_image.m_data_byte_count;
        ++i;
      }
      descriptor_write_data = vk::WriteDescriptorSet {*m_pipeline_layout.combined_image_sampler_set(), 0, 0,
                                                      vk::DescriptorType::eCombinedImageSampler, descriptor_image_infos};
      m_logical_device->updateDescriptorSets(descriptor_write_data, {});
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
      m_void_descriptor_indices {},
      m_user_interface {dear_imgui_ct {a_window, m_context, m_instance, m_physical_device, m_logical_device,
                                       m_queue_families.graphics(), m_graphics_queue, m_swapchain}},
      m_camera {camera_ct::configuration_st<camera_projection_et::e_perspective>::default_configuration()},
      m_configuration {a_configuration} {
      auto scene_cfg = input::input_subsystem_st::filesystem_st::scene_processing_configuration_st::default_configuration();
      scene_cfg.m_device_buffer = &m_common_buffer;

      input::read_scene_file("/home/billy/dev/mwc/data/mesh/metal_sphere.glb",
                             {{false, true, true}, {false, false, true}, &m_common_buffer});
      contract_assert(input::input_subsystem_st::filesystem_st::scene_registry.size() > 0);

      auto& frame_data = m_frame_synchronizer.m_synchronization_data[m_frame_synchronizer.m_frame_index];
      const auto& cmd = frame_data.m_command_buffer;

      cmd.begin(vk::CommandBufferBeginInfo {vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
      record_scene_data_transfer_to_device_memory(input::input_subsystem_st::filesystem_st::scene_registry[0], cmd);
      cmd.end();
      vk::CommandBufferSubmitInfo submit_info {*cmd};
      m_graphics_queue->submit2(vk::SubmitInfo2 {vk::SubmitFlags {}, {}, submit_info, {}});
      m_graphics_queue->waitIdle();
      m_common_buffer.release_suballocation(input::input_subsystem_st::filesystem_st::scene_registry[0].m_memory_mapped_mesh_data);
      m_common_buffer.release_suballocation(
        input::input_subsystem_st::filesystem_st::scene_registry[0].m_memory_mapped_image_data);

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
      auto camera_trans_comp = m_camera.components<ecs::transformation_st, ecs::camera_projection_st, ecs::camera_type_st>();
      const auto cursor_position_delta_x = float(input::input_subsystem_st::mouse_st::current_cursor_position.m_x
                                                 - input::input_subsystem_st::mouse_st::previous_cursor_position.m_x);
      const auto cursor_position_delta_y = float(input::input_subsystem_st::mouse_st::current_cursor_position.m_y
                                                 - input::input_subsystem_st::mouse_st::previous_cursor_position.m_y);

      auto& camera_trans = std::get<0>(camera_trans_comp).m_transformation;
      auto prev_inv_camera_trans = camera_trans /*.inverse()*/;
      auto R = camera_trans.matrix();
      const auto& camera_proj = std::get<1>(camera_trans_comp).m_projection;

      auto cam_prev_view = float3 {camera_trans(0, 2), camera_trans(1, 2), camera_trans(2, 2)};
      auto previous_rot_x = float {camera_trans(0, 2)};
      auto previous_rot_y = float {camera_trans(1, 2)};
      //auto previous_angle_axis = camera_proj.rotation().angleAxis();
      auto previous_yaw = (float)std::atan2(R(2, 1), R(1, 1));
      auto previous_pitch = (float)std::atan2(-R(3, 1), std::sqrt(std::pow(R(3, 2), 2) + std::pow(R(3, 3), 2)));

      auto new_rot_x = std::clamp(previous_rot_x + geometry::radians(cursor_position_delta_x),
                                  geometry::radians(-90.0f),
                                  geometry::radians((90.0f)));
      auto new_rot_y = previous_rot_y;

      const auto x_angle_axis
        = math::angle_axis_t<float32_t> {geometry::radians(cursor_position_delta_y), math::vector_t<float32_t, 3>::UnitX()};
      const auto y_angle_axis
        = math::angle_axis_t<float32_t> {geometry::radians(cursor_position_delta_x), math::vector_t<float32_t, 3>::UnitY()};
      auto comb_angle_axis = y_angle_axis * x_angle_axis;
      auto cam_quat_x = math::quaternion_t<float32_t> {x_angle_axis};
      cam_quat_x.normalize();
      auto cam_quat_y = math::quaternion_t<float32_t> {y_angle_axis};
      cam_quat_y.normalize();

      auto cam3x3 = math::matrix_t<float, 3, 3> {camera_proj.matrix().topLeftCorner(3, 3)};
      auto cam_roll_pitch_yaw = cam3x3.canonicalEulerAngles(2, 1, 0);

      geometry::transformation_t<float> t;

      //t.rotate() t* cam_quat_x;
      //auto full_quat = cam_quat_x * cam_quat_y;
      //auto full_rot_mat = full_quat.toRotationMatrix();
      auto camera_angle_axis = math::angle_axis_t<float> {math::quaternion_t<float> {camera_trans.rotation()}};
      auto camera_quat = math::quaternion_t<float32_t> {prev_inv_camera_trans.rotation()};
      auto new_quat = cam_quat_y * camera_quat * cam_quat_x;
      auto new_rot_mat = new_quat.matrix();

      math::vector_t<float32_t, 3> cam_pos = prev_inv_camera_trans.translation();
      //float3 cam_look_center = {0.0f, 0.0f, 0.0f};
      math::quaternion_t<float32_t> full_rot = cam_quat_y * camera_quat * cam_quat_x;
      auto full_angle_axis = math::angle_axis_t<float> {full_rot};
      //full_angle_axis.axis().x() = std::clamp(full_angle_axis.axis().x(), -1.0f, 1.0f);
      //full_angle_axis.axis().normalize();
      //full_rot = full_angle_axis;
      //full_rot.x() = std::clamp(full_rot.x(), , );
      auto full_rot_mat = geometry::transformation_t<float> {full_rot};
      //camera_trans.rotate(full_rot);

      auto view = float3 {camera_trans(0, 2), camera_trans(1, 2), camera_trans(2, 2)};
      auto added_quat = quat {};
      added_quat.vec() = cam_quat_x.vec() + cam_quat_y.vec();
      added_quat.w() = (cam_quat_x.w() + cam_quat_y.w()) * -1;
      added_quat.normalize();
      view = comb_angle_axis * view;

      auto translation = camera_trans.translation();
      //math::vector_t<float32_t, 3> {camera_trans(0, 2), camera_trans(1, 2), camera_trans(2, 2)};
      //camera_trans.rotate();
      //camera_trans.translate(translation);
      auto xxx = camera_trans.matrix();
      auto cam_view_dir = float3 {xxx(0, 2), xxx(1, 2), xxx(2, 2)};
      //const auto inv = camera_trans.matrix();
      cam_view_dir.normalize();
      auto cam_right_dir = cam_view_dir.cross(math::vector_t<float32_t, 3> {0.0f, 1.0f, 0.0f});
      cam_right_dir.normalize();

      camera_trans.rotate(comb_angle_axis);
      view = {camera_trans(0, 2), camera_trans(1, 2), camera_trans(2, 2)};
      auto up = float3 {camera_trans(0, 1), camera_trans(1, 1), camera_trans(2, 1)};
      auto right = float3 {view.cross(up)};
      right.normalize();
      constexpr auto speed = 0.01f;

      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eW)) {
        translation += view * speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eS)) {
        translation += view * -speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eD)) {
        translation += right * speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eA)) {
        translation += right * -speed;
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eSpace)) {
        translation += (float3 {0.0f, 1.0f, 0.0f} * speed);
      }
      if (input::input_subsystem_st::keyboard_st::key_map.contains(vkfw::Key::eC)) {
        translation += (float3 {0.0f, 1.0f, 0.0f} * -speed);
      }
      /*
      camera_trans.matrix().col(0) = float4 {right.x(), right.y(), right.z(), 0.0f};
      camera_trans.matrix().col(1) = float4 {up.x(), up.y(), up.z(), 0.0f};
      camera_trans.matrix().col(2) = float4 {view.x(), view.y(), view.z(), 0.0f};
      camera_trans.matrix().col(3) = float4 {translation.x(), translation.y(), translation.z(), 1.0f};
*/
      camera_trans.translation() = translation;
      //camera_trans.fromPositionOrientationScale(translation, view, math::vector_t<float32_t, 3> {1.0f, 1.0f, 1.0f});
      //camera_trans.translate(cam_pos);
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

      // mesh to render
      const auto& mesh = input::input_subsystem_st::filesystem_st::scene_registry[0].m_meshes[0];

      // push constants
      auto push_constants = vulkan::push_constant_st {};
      push_constants.m_model = camera_proj.matrix() * inv_camera.matrix()
        /* * math::matrix_t<float32_t, 4, 4>::Ones()*/;
      push_constants.m_registers.m_registers[0] = m_vertex_buffers[0].address() + mesh.m_device_mesh.m_vertex_buffer.m_offset;
      push_constants.m_material_data = {1, 2, 0};
      push_constants.m_view_data.m_view_direction
        = math::vector_t<float32_t, 4> {inv_camera(0, 2), inv_camera(1, 2), inv_camera(2, 2), 1.0f};
      push_constants.m_view_data.m_view_position = math::vector_t<float32_t, 4> {cam_pos[0], cam_pos[1], cam_pos[2], 0.0f};
      push_constants.m_current_time = vkfw::getTime().value;
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

      // vertex buffer
      /*
            auto vert_attribs = array_t<vk::VertexInputAttributeDescription2EXT, 4> {};
            vert_attribs[0] = {0, 0, vk::Format::eR32G32B32Sfloat, 0};
            vert_attribs[1] = {1, 0, vk::Format::eR32G32B32Sfloat, 12};
            vert_attribs[2] = {2, 0, vk::Format::eR32G32B32A32Sfloat, 24};
            vert_attribs[3] = {3, 0, vk::Format::eR32G32Sfloat, 40};
            cmd.bindVertexBuffers(0, mesh.m_device_mesh.m_vertex_buffer.m_buffer, {0});
            cmd.setVertexInputEXT(vk::VertexInputBindingDescription2EXT {0, uint32_t(mesh.m_host_mesh.m_vertex_model_size),
                                                                         vk::VertexInputRate::eVertex, 1},
                                  vert_attribs);*/
      // descriptors
      cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline_layout.native_handle(), 0,
                             *m_pipeline_layout.combined_image_sampler_set(), {});
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