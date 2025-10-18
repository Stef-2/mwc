#pragma once

import mwc_input_resource;
import mwc_host_mesh;
import mwc_device_mesh;

namespace mwc {
  namespace input {
    struct mesh_st : public resource_st {
      dynamic_host_mesh_st m_host_mesh;
      graphics::device_mesh_st m_device_mesh;
    };
  }
}