module;

#include "mwc/core/filesystem/file_type.hpp"

export module mwc_input_resource;

import mwc_definition;
import mwc_set_bit;
import mwc_bit_mask;
import std;

export namespace mwc {
  namespace input {
    using resource_filepath_t = file_path_t;
    using resource_name_t = string_t;
    using resource_modification_time_t = time_point_t;
    using resource_index_t = uint32_t;

    enum class resource_type_et : uint8_t {
      e_mesh,
      e_camera,
      e_light,
      e_image,
      e_shader,
      end
    };
    enum class resource_linkage_et : uint8_t {
      e_internal,
      e_external
    };
    enum class resource_bit_flags_et : uint8_t {
      e_dependent = utility::set_bit<1>()
    };

    template <resource_type_et tp_resource_type, resource_linkage_et tp_linkage = resource_linkage_et::e_external>
    struct static_resource_information_st {};

    struct resource_st {
      resource_st(const file_path_t& a_filepath, const string_view_t a_name = "",
                  const resource_linkage_et a_linkage = resource_linkage_et::e_internal,
                  const bit_mask_t<resource_bit_flags_et> a_bit_flags = {})
      : m_source_filepath {a_filepath},
        m_name {a_name.empty() ? a_filepath.filename().string() : a_name},
        m_modification_time {std::filesystem::last_write_time(a_filepath).time_since_epoch()},
        m_linkage {a_linkage},
        m_bit_flags {a_bit_flags} {}

      auto operator<=>(const resource_st& a_other) const {
        return m_name <=> a_other.m_name;
      }

      file_path_t m_source_filepath;
      resource_name_t m_name;
      resource_modification_time_t m_modification_time;
      resource_linkage_et m_linkage;
      bit_mask_t<resource_bit_flags_et> m_bit_flags;
    };

    void test(file_path_t a_filepath) {
      std::format("{0}", a_filepath.string());
    }
  }
}