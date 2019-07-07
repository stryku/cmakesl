#pragma once

#include "ast/ast_node.hpp"

namespace cmsl::ast {
class break_node : public ast_node
{
public:
  explicit break_node(token_t break_token, token_t semicolon);

  const token_t& break_() const;
  const token_t& semicolon() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  token_t m_break;
  token_t m_semicolon;
};
}
