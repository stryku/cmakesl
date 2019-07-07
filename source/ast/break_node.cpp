#include "ast/break_node.hpp"

#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast {
break_node::break_node(ast_node::token_t break_token,
                       ast_node::token_t semicolon)
  : m_break{ break_token }
  , m_semicolon{ semicolon }
{
}

const ast_node::token_t& break_node::break_() const
{
  return m_break;
}

const ast_node::token_t& break_node::semicolon() const
{
  return m_semicolon;
}

void break_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location break_node::begin_location() const
{
  return m_break.src_range().begin;
}

source_location break_node::end_location() const
{
  return m_semicolon.src_range().end;
}
}
