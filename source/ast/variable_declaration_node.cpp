#include "ast/variable_declaration_node.hpp"

#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast {
variable_declaration_node::variable_declaration_node(
  type_representation ty, token_t name,
  std::optional<initialization_values> initialization)
  : m_type{ std::move(ty) }
  , m_name{ name }
  , m_initialization{ std::move(initialization) }
{
}

const type_representation& variable_declaration_node::type() const
{
  return m_type;
}

variable_declaration_node::token_t variable_declaration_node::name() const
{
  return m_name;
}

std::optional<variable_declaration_node::token_t>
variable_declaration_node::equal() const
{
  if (m_initialization) {
    return m_initialization->equal;
  }

  return std::nullopt;
}

const ast_node* variable_declaration_node::initialization() const
{
  return m_initialization ? m_initialization->initialization.get() : nullptr;
}

void variable_declaration_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location variable_declaration_node::begin_location() const
{
  return m_type.tokens().front().src_range().begin;
}

source_location variable_declaration_node::end_location() const
{
  if (m_initialization) {
    return m_initialization->initialization->end_location();
  } else {
    return m_name.src_range().end;
  }
}

standalone_variable_declaration_node::standalone_variable_declaration_node(
  std::unique_ptr<variable_declaration_node> variable_decl, token_t semicolon)
  : m_variable_decl{ std::move(variable_decl) }
  , m_semicolon{ semicolon }
{
}

void standalone_variable_declaration_node::visit(
  ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location standalone_variable_declaration_node::end_location() const
{
  return m_semicolon.src_range().end;
}

const variable_declaration_node&
standalone_variable_declaration_node::variable_declaration() const
{
  return *m_variable_decl;
}

const ast_node::token_t& standalone_variable_declaration_node::semicolon()
  const
{
  return m_semicolon;
}
source_location standalone_variable_declaration_node::begin_location() const
{
  return m_variable_decl->begin_location();
}
}
