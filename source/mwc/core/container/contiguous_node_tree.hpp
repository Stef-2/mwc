#pragma once

import mwc_definition;

import std;

namespace mwc {
  template <typename tp, std::unsigned_integral tp_index = uint16_t>
  class contiguous_node_tree_ct {
    public:
    using data_t = tp;
    using index_t = tp_index;
    using node_storage_t = vector_t<struct node_st>;

    struct node_st {
      data_t m_data;
      index_t m_parent_node;
      index_t m_children_count;
      index_t m_child_node;
    };

    private:
    node_storage_t m_nodes;
  };
}
