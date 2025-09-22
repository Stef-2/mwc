module;

export module mwc_input_resource;

import mwc_definition;
import mwc_set_bit;
import mwc_bit_mask;

export namespace mwc {
  namespace input {
    using resource_name_t = string_t;
    using resource_modification_time_t = time_point_t;
    using resource_index_t = uint32_t;

    enum class resource_type_et : uint8_t {
      e_mesh,
      e_camera,
      e_light,
      e_image,
      e_shader
    };
    enum class resource_linkage_et : uint8_t {
      e_internal,
      e_external
    };
    enum class resource_bit_flags_et : uint8_t {
      e_dependent = utility::set_bit<1>()
    };
    struct resource_st {
      resource_name_t m_name;
      resource_modification_time_t m_modification_time;
      resource_type_et m_type;
      resource_linkage_et m_linkage;
      bit_mask_t<resource_bit_flags_et> m_bit_flags;
    };
  }
}