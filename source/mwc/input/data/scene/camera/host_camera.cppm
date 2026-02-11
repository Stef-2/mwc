module;

export module mwc_host_camera;

import mwc_geometry;
import mwc_camera_projection_type;

export namespace mwc {
  namespace input {
    struct camera_st {
      geometry::camera_projection_t m_projection;
      graphics::camera_projection_et m_projection_type;
    };
  }
}