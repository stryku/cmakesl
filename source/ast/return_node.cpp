#include "ast/return_node.hpp"
#include "ast/ast_node_visitor.hpp"
#include "return_node.hpp"

namespace cmsl::ast {
return_node::return_node(token_t return_kw, std::unique_ptr<ast_node> expr,
                         token_t semicolon)
  : m_return_kw{ return_kw }
  , m_expression{ std::move(expr) }
  , m_semicolon{ semicolon }
{
}

const ast_node& return_node::expression() const
{
  return *m_expression;
}

void return_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

const ast_node::token_t& return_node::return_kw() const
{
  return m_return_kw;
}

const ast_node::token_t& return_node::semicolon() const
{
  return m_semicolon;
}

source_location return_node::begin_location() const
{
  return m_return_kw.src_range().begin;
}

source_location return_node::end_location() const
{
  return m_semicolon.src_range().end;
}
}
