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
    std::optional<token_t> export_kw, type_representation ty, token_t name,
    std::optional<initialization_values> initialization);

  const std::optional<token_t>& export_() const;
  const type_representation& type() const;
  token_t name() const;
  token_t semicolon() const;
  std::optional<token_t> equal() const;
  const ast_node* initialization() const;

  bool is_exported() const override;
  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  std::optional<token_t> m_export_kw;
  type_representation m_type;
  lexer::token m_name;
  std::optional<initialization_values> m_initialization;
};

class standalone_variable_declaration_node : public ast_node
{
public:
  using initialization_values_t =
    variable_declaration_node::initialization_values;

  explicit standalone_variable_declaration_node(
    std::unique_ptr<variable_declaration_node> variable_decl,
    token_t semicolon);

  const variable_declaration_node& variable_declaration() const;
  const token_t& semicolon() const;

  bool is_exported() const override;
  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  std::unique_ptr<variable_declaration_node> m_variable_decl;
  token_t m_semicolon;
};
}
