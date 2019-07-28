#pragma once

#include "ast/ast_node.hpp"
#include "ast/qualified_name.hpp"

#include <memory>
#include <vector>

namespace cmsl::ast {

class namespace_node : public ast_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<ast_node>>;
  using names_t = std::vector<name_with_coloncolon>;

  explicit namespace_node(token_t namespace_kw, names_t names,
                          token_t open_brace, nodes_t nodes,
                          token_t close_brace);

  const token_t& namespace_() const;
  const names_t& names() const;
  const token_t& open_brace() const;
  const nodes_t& nodes() const;
  const token_t& close_brace() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  token_t m_namespace;
  names_t m_names;
  token_t m_open_brace;
  nodes_t m_nodes;
  token_t m_close_brace;
};
}
