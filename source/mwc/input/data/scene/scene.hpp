#pragma once

#include "mwc/core/container/contiguous_node_tree.hpp"
#include "mwc/ecs/definition.hpp"

import mwc_definition;
import mwc_input_resource;
import mwc_geometry;
import mwc_host_mesh;
import mwc_input_image;

import std;

namespace mwc {
  namespace input {
    struct scene_st : public resource_st {
      struct node_data_st {
        using index_t = uint16_t;

        static constexpr auto null_mesh_index = std::numeric_limits<index_t>::max();

        geometry::transformation_t<> m_transformation;
        index_t m_mesh_index;
      };
      using mesh_storage_t = vector_t<dynamic_host_mesh_st>;
      using node_storage_t = contiguous_node_tree_ct<node_data_st, node_data_st::index_t>;
      using image_storage_t = vector_t<dynamic_image_st>;

      using resource_st::resource_st;
      using resource_st::operator<=>;

      mesh_storage_t m_meshes;
      node_storage_t m_nodes;
      image_storage_t m_images;
    };
  }
}