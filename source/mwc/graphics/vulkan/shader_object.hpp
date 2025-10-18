#pragma once

#include "mwc/graphics/vulkan/logical_device.hpp"
#include "mwc/graphics/vulkan/physical_device.hpp"

#include <vulkan/vulkan_core.h>

import mwc_definition;
import mwc_extent;
import mwc_type_mobility;

import vulkan_hpp;

import std;

namespace mwc {
  namespace graphics {
    namespace vulkan {
      struct shader_object_binary_compatibility_st {
        using binary_uuid_t = decltype(vk::PhysicalDeviceShaderObjectPropertiesEXT::shaderBinaryUUID);
        using binary_version_t = decltype(vk::PhysicalDeviceShaderObjectPropertiesEXT::shaderBinaryVersion);

        auto assert_compatibility(const shader_object_binary_compatibility_st& a_other) const -> bool_t;

        binary_uuid_t m_uuid;
        binary_version_t m_version;
      };
      template <size_t tp_extent = std::dynamic_extent>
      class shader_object_pipeline_ct : public irreproducible_st {
        public:
        using storage_t = extent_t<vk::ShaderEXT, tp_extent>;

        struct configuration_st {
          using shader_create_info_storage_t = extent_t<vk::ShaderCreateInfoEXT, tp_extent>;

          shader_create_info_storage_t m_shader_create_info;
        };

        shader_object_pipeline_ct(const logical_device_ct& a_logical_device,
                                  const physical_device_ct& a_physical_device,
                                  const configuration_st& a_configuration);

        shader_object_pipeline_ct(shader_object_pipeline_ct&& a_other) noexcept = default;
        auto operator=(shader_object_pipeline_ct&& a_other) noexcept -> shader_object_pipeline_ct& = default;
        ~shader_object_pipeline_ct();

        auto binary_compatibility() const -> const shader_object_binary_compatibility_st&;
        auto binary_data() const -> extent_t<vector_t<byte_t>, tp_extent>;
        template <typename tp_this>
        auto shader_objects(this tp_this&& a_this) -> decltype(auto);
        template <typename tp_this>
        constexpr auto configuration(this tp_this&& a_this) -> decltype(auto);

        private:
        const logical_device_ct& m_logical_device;
        storage_t m_storage;
        shader_object_binary_compatibility_st m_binary_compatibility;
        configuration_st m_configuration;
      };

      // implementation
      template <size_t tp_extent>
      shader_object_pipeline_ct<tp_extent>::shader_object_pipeline_ct(const logical_device_ct& a_logical_device,
                                                                      const physical_device_ct& a_physical_device,
                                                                      const configuration_st& a_configuration)
      : m_logical_device {a_logical_device},
        m_storage {std::invoke([this, &a_logical_device, &a_configuration] -> storage_t {
          auto storage = storage_t {};
          if constexpr (tp_extent == std::dynamic_extent)
            storage.resize(a_configuration.m_shader_create_info.size());

          const auto native_create_info_ptr
            = std::bit_cast<const VkShaderCreateInfoEXT*>(a_configuration.m_shader_create_info.data());
          const auto native_storage_ptr = std::bit_cast<VkShaderEXT*>(storage.data());

          const auto result = static_cast<vk::Result>(m_logical_device->getDispatcher()->vkCreateShadersEXT(
            m_logical_device.native_handle(), a_configuration.m_shader_create_info.size(), native_create_info_ptr, nullptr,
            native_storage_ptr));
          contract_assert(result == vk::Result::eSuccess);

          for (const auto& shader_object : storage)
            contract_assert(shader_object);

          return storage;
        })},
        m_binary_compatibility {
        .m_uuid
        = a_physical_device.properties().m_default_properties_chain.get<vk::PhysicalDeviceShaderObjectPropertiesEXT>().shaderBinaryUUID,
        .m_version = a_physical_device.properties()
                       .m_default_properties_chain.get<vk::PhysicalDeviceShaderObjectPropertiesEXT>()
                       .shaderBinaryVersion} {}

      template <size_t tp_extent>
      shader_object_pipeline_ct<tp_extent>::~shader_object_pipeline_ct() {
        // assume that if the first shader object in the pipeline exists, they all do
        if (m_storage.front())
          for (const auto& shader_object : m_storage)
            m_logical_device->getDispatcher()->vkDestroyShaderEXT(m_logical_device.native_handle(), shader_object, nullptr);
      }
      template <size_t tp_extent>
      auto shader_object_pipeline_ct<tp_extent>::binary_data() const -> extent_t<vector_t<byte_t>, tp_extent> {
        auto binary_data = extent_t<vector_t<byte_t>, tp_extent> {};
        if constexpr (tp_extent == std::dynamic_extent)
          binary_data.reserve(m_storage.size());

        const auto& dispatcher = m_logical_device->getDispatcher();
        for (auto i = 0uz; i < m_storage.size(); ++i) {
          auto size = vk::DeviceSize {};
          auto result = static_cast<vk::Result>(
            dispatcher->vkGetShaderBinaryDataEXT(m_logical_device.native_handle(), m_storage[i], &size, nullptr));
          contract_assert(result == vk::Result::eSuccess);
          binary_data[i] = vector_t<byte_t>(size);
          result = static_cast<vk::Result>(
            dispatcher->vkGetShaderBinaryDataEXT(m_logical_device.native_handle(), m_storage[i], &size, binary_data[i].data()));
          contract_assert(result == vk::Result::eSuccess);
        }

        return binary_data;
      }
      template <size_t tp_extent>
      template <typename tp_this>
      auto shader_object_pipeline_ct<tp_extent>::shader_objects(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(span_t {a_this.m_storage});
      }
      template <size_t tp_extent>
      template <typename tp_this>
      constexpr auto shader_object_pipeline_ct<tp_extent>::configuration(this tp_this&& a_this) -> decltype(auto) {
        return std::forward_like<decltype(a_this)>(a_this.m_configuration);
      }

      inline void test() {
        logical_device_ct* ld;
        physical_device_ct* pd;
        //pipeline_layout_ct* pl;
        shader_object_pipeline_ct<2> xxx {*ld, *pd, {}};

        shader_object_pipeline_ct<2> yyy = std::move(xxx);
        optional_t<shader_object_pipeline_ct<>> osop {};
      }
    }
  }
}
