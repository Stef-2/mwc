#pragma once

#include "mwc/core/container/contiguous_node_tree.hpp"
#include "mwc/input/data/scene/mesh.hpp"
#include "mwc/input/data/scene/image/image.hpp"

import mwc_definition;
import mwc_geometry;

import std;

namespace mwc {
  namespace input {
    struct scene_st {
      using suballocation_t = data_span_st<byte_t>;

      struct node_data_st {
        geometry::transformation_t<> m_transformation;
        resource_index_t m_resource_index;
      };
      using mesh_storage_t = vector_t<mesh_st>;
      using image_storage_t = vector_t<image_st>;
      using node_storage_t = contiguous_node_tree_ct<node_data_st, resource_index_t>;

      mesh_storage_t m_meshes;
      suballocation_t m_memory_mapped_mesh_data;
      image_storage_t m_images;
      suballocation_t m_memory_mapped_image_data;
      node_storage_t m_nodes;
    };
  }
}