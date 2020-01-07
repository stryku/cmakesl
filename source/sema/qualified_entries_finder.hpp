#pragma once

#include "ast/qualified_name.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "errors/error.hpp"
#include "errors/errors_observer.hpp"
#include "lexer/token.hpp"

#include <algorithm>

namespace cmsl::sema {
template <typename Entry>
class qualified_entries_finder
{
private:
  using token_t = lexer::token;
  struct possibly_exported_entry
  {
    Entry e;
    bool exported{ false };
  };
  using entries_map_t =
    std::unordered_multimap<token_t, possibly_exported_entry>;
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
    bool exported{ false };
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
    (void)create_node(name_token, false);
    m_current_nodes_path.push_back({ name_token, 0u });
  }

  void register_entry(token_t name, Entry entry, bool exported)
  {
    possibly_exported_entry e{ .e = std::move(entry), .exported = exported };
    current_entries().emplace(name, std::move(e));
  }

  void enter_global_node(token_t name, bool exported)
  {
    node_id_t node_id;

    auto current = &current_node();

    if (contains(current->nodes, name)) {
      node_id = current->nodes[name];
    } else {
      node_id = create_node(name, exported);
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

  bool is_in_global_context() const { return m_local_nodes.empty(); }

  struct entry_info
  {
    token_t registration_token;
    Entry entry;
    bool exported{ false };
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

  // Todo: test it
  qualified_entries_finder collect_exported_stuff() const
  {
    qualified_entries_finder cloned;
    cloned.m_nodes_container = m_nodes_container;
    cloned.erase_not_exported();
    return cloned;
  }

  bool merge_imported_stuff(const qualified_entries_finder& imported,
                            errors::errors_observer& errs)
  {
    return merge_node(imported.m_nodes_container,
                      imported.m_nodes_container[0], m_nodes_container[0]);
  }

private:
  bool merge_node(std::vector<tree_node> imported_nodes_container,
                  const tree_node& imported_node, tree_node& into)
  {
    bool result{ true };

    // We don't want recursive imports, thus the false.
    const auto exported{ false };

    entries_map_t merged_entries;

    for (const auto& [token, entry] : imported_node.entries) {
      const auto found = into.entries.find(token);
      if (found != std::cend(into.entries)) {
        // Todo: redeclaration
        result = false;
        continue;
      }

      possibly_exported_entry e{ .e = entry.e, .exported = exported };
      merged_entries.emplace(token, std::move(e));
    }

    auto& curr_entries = current_entries();
    std::move(std::begin(merged_entries), std::end(merged_entries),
              std::inserter(curr_entries, std::end(curr_entries)));

    for (const auto& [token, id] : imported_node.nodes) {
      (void)token;
      const auto& node = imported_nodes_container[id];
      enter_global_node(node.name, exported);
      const auto node_merging_result =
        merge_node(imported_nodes_container, node, current_node());
      result = result && node_merging_result;
      leave_node();
    }

    return result;
  }

  void erase_not_exported()
  {
    auto& root_node = m_nodes_container[0];
    erase_not_exported_in_node(root_node);
  }

  void erase_not_exported_in_node(tree_node& node)
  {
    const auto entry_pred = [](const auto& pair) {
      const auto& entry = pair.second;
      return !entry.exported;
    };

    cmsl::remove_if(node.entries, entry_pred);

    for (auto& child : node.nodes) {
      const auto node_id = child.second;
      auto& child_node = m_nodes_container[node_id];
      erase_not_exported_in_node(child_node);
    }
  }

  std::vector<entry_info> to_entries_info(
    const search_result_range_t& range) const
  {
    std::vector<entry_info> results;

    std::transform(range.first, range.second, std::back_inserter(results),
                   [](const auto& pair) {
                     const auto& [registration_token, exported_entry] = pair;

                     return entry_info{ .registration_token =
                                          registration_token,
                                        .entry = exported_entry.e,
                                        .exported = exported_entry.exported };
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

  node_id_t create_node(token_t name, bool exported)
  {
    const node_id_t id = m_nodes_container.size();
    tree_node node{ .name = name,
                    .id = id,
                    .parent_id = current_node_id(),
                    .exported = exported };
    m_nodes_container.emplace_back(node);
    if (current_node_id() != k_bad_id) {
      auto& current = current_node();
      current.nodes[name] = id;
    }
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

    const auto is_explicit_root_node_accessed = first_name.str().empty();
    if (is_explicit_root_node_accessed) {
      node = &m_nodes_container[0];
    } else {
      while (true) {
        const auto found = node->nodes.find(first_name);
        if (found != std::cend(node->nodes)) {
          const auto id = found->second;
          node = &m_nodes_container[id];
          break;
        }

        const auto is_root_node = (node->id == 0u);
        if (is_root_node) {
          node = nullptr;
          break;
        }

        node = &m_nodes_container[node->parent_id];
      }
    }

    if (node == nullptr) {
      return std::nullopt;
    }

    auto first = std::next(begin);
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
