module;

export module mwc_camera_projection_type;

import mwc_definition;

export namespace mwc {
  namespace graphics {
    enum class camera_projection_et : uint8_t {
      e_perspective,
      e_orthographic,
      end
    };
  }
}