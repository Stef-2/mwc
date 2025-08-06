module;

export module mwc_input_scene;

import mwc_definition;
import mwc_input_mesh;

export namespace mwc {
  namespace input {
    struct scene_st {
      using mesh_storage_t = vector_t<dynamic_mesh_st>;

      mesh_storage_t m_meshes;
    };
  }
}
