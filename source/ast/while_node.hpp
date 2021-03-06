#pragma once

#include "ast/ast_node.hpp"

#include <memory>

namespace cmsl::ast {
class conditional_node;

class while_node : public ast_node
{
public:
  explicit while_node(token_t while_kw,
                      std::unique_ptr<conditional_node> node);
  ~while_node();

  token_t while_kw() const;

  const conditional_node& node() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  token_t m_while_kw;
  std::unique_ptr<conditional_node> m_node;
};
}
