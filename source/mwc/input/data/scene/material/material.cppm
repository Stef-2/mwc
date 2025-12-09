module;

#include "mwc/input/data/scene/image/image.hpp"

export module mwc_material;

import mwc_definition;
import mwc_observer_ptr;
import mwc_color;

import std;

export namespace mwc {
  namespace input {
    struct material_st {
      obs_ptr_t<image_st> m_base_color_map;
      color_st<float32_t> m_base_color;

      obs_ptr_t<image_st> m_metallic_roughness_map;
      float32_t m_metallic_value;
      float32_t m_roughness_value;

      obs_ptr_t<image_st> m_tangent_space_normal_map;

      obs_ptr_t<image_st> m_specular_map;
      color_st<float32_t> m_specular_color;
    };
  }
}