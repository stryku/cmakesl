#include "ast/designated_initializers_node.hpp"
#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast {
const ast_node::token_t& designated_initializers_node::open_brace() const
{
  return m_open_brace;
}

const designated_initializers_node::initializers_t&
designated_initializers_node::initializers() const
{
  return m_initializers;
}

const ast_node::token_t& designated_initializers_node::close_brace() const
{
  return m_close_brace;
}

void designated_initializers_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location designated_initializers_node::begin_location() const
{
  return m_open_brace.src_range().begin;
}

source_location designated_initializers_node::end_location() const
{
  return m_close_brace.src_range().end;
}
}
