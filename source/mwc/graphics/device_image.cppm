module;

#include "mwc/graphics/vulkan/image.hpp"

export module mwc_device_image;

import vulkan_hpp;

export namespace mwc {
  namespace graphics {
    struct device_image_st {
      using device_image_index_t = uint32_t;

      vulkan::image_ct m_image {nullptr};
      vk::raii::ImageView m_image_view {nullptr};
      vk::raii::Sampler m_sampler {nullptr};
      device_image_index_t m_image_index {std::numeric_limits<device_image_index_t>::max()};
    };
  }
}