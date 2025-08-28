#pragma once

import mwc_definition;

import std;

namespace mwc {
  template <typename tp, std::unsigned_integral tp_index = uint16_t>
  class contiguous_node_tree_ct {
    public:
    using data_t = tp;
    using index_t = tp_index;

    static constexpr auto root_node_index = index_t {0};

    struct node_st {
      data_t m_data;
      index_t m_parent_node;
    };

    using node_storage_t = vector_t<node_st>;

    struct configuration_st {
      static constexpr auto default_configuration() -> const configuration_st;

      size_t m_initial_capacity;
    };

    contiguous_node_tree_ct(const configuration_st& a_configuration = configuration_st::default_configuration());

    auto insert_node(const data_t& a_data, const index_t a_parent_index = root_node_index) -> index_t
      pre(a_parent_index <= node_count());
    auto parent(const index_t a_index) const -> index_t pre(a_index < node_count());
    auto children(const index_t a_index) const -> vector_t<index_t> pre(a_index < node_count());
    auto node_count() const -> index_t;
    template <typename tp_this>
    auto operator[](this tp_this&& a_this, const index_t a_index) -> decltype(auto) pre(a_index < node_count());
    template <typename tp_this>
    auto begin(this tp_this&& a_this) -> decltype(auto);
    template <typename tp_this>
    auto end(this tp_this&& a_this) -> decltype(auto);

    private:
    node_storage_t m_nodes;
  };

  // implementation
  template <typename tp, std::unsigned_integral tp_index>
  constexpr auto contiguous_node_tree_ct<tp, tp_index>::configuration_st::default_configuration() -> const configuration_st {
    return configuration_st {.m_initial_capacity = 32};
  }
  template <typename tp, std::unsigned_integral tp_index>
  contiguous_node_tree_ct<tp, tp_index>::contiguous_node_tree_ct(const configuration_st& a_configuration) : m_nodes {} {
    m_nodes.reserve(a_configuration.m_initial_capacity);
  }
  template <typename tp, std::unsigned_integral tp_index>
  auto contiguous_node_tree_ct<tp, tp_index>::insert_node(const tp& a_data, const index_t a_parent_index) -> index_t {
    m_nodes.emplace_back(a_data, a_parent_index);

    return m_nodes.size() - 1;
  }
  template <typename tp, std::unsigned_integral tp_index>
  auto contiguous_node_tree_ct<tp, tp_index>::parent(const index_t a_index) const -> index_t {
    return m_nodes[a_index].m_parent_node;
  }
  template <typename tp, std::unsigned_integral tp_index>
  auto contiguous_node_tree_ct<tp, tp_index>::children(const index_t a_index) const -> vector_t<index_t> {
    auto children = vector_t<index_t> {};

    for (auto i = index_t {a_index}; i < m_nodes.size(); ++i)
      if (m_nodes[i].m_parent_node == a_index)
        children.push_back(m_nodes[i].m_index);

    return children;
  }
  template <typename tp, std::unsigned_integral tp_index>
  auto contiguous_node_tree_ct<tp, tp_index>::node_count() const -> index_t {
    return m_nodes.size();
  }
  template <typename tp, std::unsigned_integral tp_index>
  template <typename tp_this>
  auto contiguous_node_tree_ct<tp, tp_index>::operator[](this tp_this&& a_this, const index_t a_index) -> decltype(auto) {
    return std::forward_like<decltype(a_this)>(a_this.m_nodes[a_index].m_data);
  }
  template <typename tp, std::unsigned_integral tp_index>
  template <typename tp_this>
  auto contiguous_node_tree_ct<tp, tp_index>::begin(this tp_this&& a_this) -> decltype(auto) {
    if constexpr (std::is_const_v<decltype(a_this)>)
      return a_this.m_nodes.cbegin();
    else
      return a_this.m_nodes.begin();
  }
  template <typename tp, std::unsigned_integral tp_index>
  template <typename tp_this>
  auto contiguous_node_tree_ct<tp, tp_index>::end(this tp_this&& a_this) -> decltype(auto) {
    if constexpr (std::is_const_v<decltype(a_this)>)
      return a_this.m_nodes.cend();
    else
      return a_this.m_nodes.end();
  }
}