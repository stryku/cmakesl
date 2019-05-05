#pragma once

#include "ast/ast_node.hpp"

#include <memory>
#include <vector>

namespace cmsl::ast {
class translation_unit_node : public ast_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<ast_node>>;

  explicit translation_unit_node(nodes_t nodes);

  const nodes_t& nodes() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  nodes_t m_nodes;
};
}