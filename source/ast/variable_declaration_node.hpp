#pragma once

#include "ast/ast_node.hpp"
#include "ast/type_representation.hpp"

#include <memory>

namespace cmsl::ast {
class variable_declaration_node : public ast_node
{
public:
  struct initialization_values
  {
    token_t equal;
    std::unique_ptr<ast_node> initialization;
  };

  explicit variable_declaration_node(
    type_representation ty, token_t name,
    std::optional<initialization_values> initialization, token_t semicolon);

  const type_representation& type() const;
  token_t name() const;
  token_t semicolon() const;
  std::optional<token_t> equal() const;
  const ast_node* initialization() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  type_representation m_type;
  lexer::token m_name;
  std::optional<initialization_values> m_initialization;
  token_t m_semicolon;
};
}
