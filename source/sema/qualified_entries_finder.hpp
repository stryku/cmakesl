#pragma once

#include "ast/qualified_name.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "lexer/token.hpp"

#include <algorithm>

namespace cmsl::sema {
template <typename Entry>
class qualified_entries_finder
{
private:
  using token_t = lexer::token;
  using entries_map_t = std::unordered_multimap<token_t, Entry>;
  using node_id_t = unsigned;
  static constexpr auto k_bad_id = std::numeric_limits<node_id_t>::max();
  using qualified_names_t = std::vector<ast::name_with_coloncolon>;
  using search_result_range_t =
    std::pair<typename entries_map_t ::const_iterator,
              typename entries_map_t ::const_iterator>;

  struct tree_node
  {
    token_t name;
    node_id_t id{ k_bad_id };
    node_id_t parent_id{ k_bad_id };
    std::unordered_map<token_t, node_id_t> nodes;
    entries_map_t entries;
  };

  struct node_id_and_name
  {
    token_t name;
    node_id_t id{};
  };

public:
  explicit qualified_entries_finder()
  {
    static const auto name_token =
      lexer::make_token(lexer::token_type::identifier, "");
    (void)create_node(name_token);
    m_current_nodes_path.push_back({ name_token, 0u });
  }

  void register_entry(token_t name, Entry entry)
  {
    current_entries().emplace(name, std::move(entry));
  }

  void enter_global_node(token_t name)
  {
    node_id_t node_id;

    auto current = &current_node();

    if (contains(current->nodes, name)) {
      node_id = current->nodes[name];
    } else {
      node_id = create_node(name);
      current = &current_node();
      current->nodes[name] = node_id;
    }

    m_current_nodes_path.push_back({ name, node_id });
  }

  void enter_local_node() { m_local_nodes.emplace_back(entries_map_t{}); }

  void leave_node()
  {
    const auto is_in_local_node = !m_local_nodes.empty();
    if (is_in_local_node) {
      m_local_nodes.pop_back();
    } else {
      m_current_nodes_path.pop_back();
    }
  }

  struct entry_info
  {
    token_t registration_token;
    Entry entry;
  };

  std::vector<entry_info> find(const qualified_names_t& names) const
  {
    const auto found_range = find_it(names);

    if (!found_range) {
      return {};
    }

    return to_entries_info(*found_range);
  }

  std::vector<entry_info> find_in_current_node(const lexer::token& name) const
  {
    const auto& entries = current_entries();

    const auto found_range = entries.equal_range(name);
    return to_entries_info(found_range);
  }

  std::optional<token_t> find_node_registration_token(
    const qualified_names_t& names) const
  {
    const auto node = find_node(std::cbegin(names), std::cend(names));
    if (!node) {
      return std::nullopt;
    }

    return node->name;
  }

private:
  std::vector<entry_info> to_entries_info(
    const search_result_range_t& range) const
  {
    std::vector<entry_info> results;

    std::transform(range.first, range.second, std::back_inserter(results),
                   [](const auto& pair) {
                     return entry_info{ pair.first, pair.second };
                   });

    return results;
  }

  tree_node& current_node()
  {
    if (const auto curr_id = current_node_id(); curr_id != k_bad_id) {
      return m_nodes_container.at(curr_id);
    } else {
      return m_nodes_container[0];
    }
  }

  const tree_node& current_node() const
  {
    if (const auto curr_id = current_node_id(); curr_id != k_bad_id) {
      return m_nodes_container.at(curr_id);
    } else {
      return m_nodes_container[0];
    }
  }

  node_id_t create_node(token_t name)
  {
    const node_id_t id = m_nodes_container.size();
    tree_node node{ name, id, current_node_id() };
    m_nodes_container.emplace_back(node);
    auto& current = current_node();
    current.nodes[name] = id;
    return id;
  }

  node_id_t current_node_id() const
  {
    return m_current_nodes_path.empty() ? k_bad_id
                                        : m_current_nodes_path.back().id;
  }

  bool is_qualified_name(const qualified_names_t& names) const
  {
    return names.size() > 1u;
  }

  std::optional<search_result_range_t> find_it(
    const qualified_names_t& names) const
  {
    if (is_qualified_name(names)) {
      return find_qualified(names);
    } else {
      const auto name = names.front().name;
      return find(name);
    }
  }

  std::optional<node_id_and_name> find_node(
    qualified_names_t::const_iterator begin,
    qualified_names_t::const_iterator end) const
  {
    const auto& first_name = begin->name;
    auto* node = &current_node();

    while (true) {
      if (contains(node->nodes, first_name)) {
        break;
      }

      const auto is_root_node = (node->id == 0u);
      if (is_root_node) {
        node = nullptr;
        break;
      }

      node = &m_nodes_container[node->parent_id];
    }

    if (node == nullptr) {
      return std::nullopt;
    }

    auto first = begin;
    while (first != end) {
      if (!contains(node->nodes, first->name)) {
        return std::nullopt;
      }

      const auto next_node_id = node->nodes.at(first->name);
      node = &m_nodes_container[next_node_id];
      ++first;
    }

    return node_id_and_name{ node->name, node->id };
  }

  std::optional<search_result_range_t> find_qualified(
    const qualified_names_t& names) const
  {
    const auto found_ctx =
      find_node(std::cbegin(names), std::prev(std::cend(names)));
    if (!found_ctx) {
      return std::nullopt;
    }

    const auto node = &m_nodes_container[found_ctx->id];
    const auto looked_identifier_name = names.back().name;

    return find_in_node(looked_identifier_name, node->entries);
  }

  std::optional<search_result_range_t> find_in_node(
    const token_t& name, const entries_map_t& ids) const
  {
    const auto found = ids.equal_range(name);
    if (found.first == std::cend(ids)) {
      return std::nullopt;
    }

    return found;
  }

  std::optional<search_result_range_t> find(const token_t& name) const
  {
    // Try to find in local nodes.
    for (auto node_it = std::crbegin(m_local_nodes);
         node_it != std::crend(m_local_nodes); ++node_it) {
      const auto found = find_in_node(name, *node_it);
      if (found) {
        return *found;
      }
    }

    // If not found, try to find in global nodes.
    auto* current = &current_node();

    while (true) {
      const auto found = find_in_node(name, current->entries);
      if (found) {
        return *found;
      }

      const auto is_root_ctx = (current->id == 0u);
      if (is_root_ctx) {
        return std::nullopt;
      }

      current = &m_nodes_container[current->parent_id];
    }
  }

  entries_map_t& current_entries()
  {
    if (!m_local_nodes.empty()) {
      return m_local_nodes.back();
    }

    return current_node().entries;
  }

  const entries_map_t& current_entries() const
  {
    if (!m_local_nodes.empty()) {
      return m_local_nodes.back();
    }

    return current_node().entries;
  }

private:
  std::vector<tree_node> m_nodes_container;
  std::vector<node_id_and_name> m_current_nodes_path;
  std::vector<entries_map_t> m_local_nodes;
};
}
