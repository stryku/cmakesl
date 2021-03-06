#include "ast/conditional_node.hpp"

#include "ast/ast_node_visitor.hpp"
#include "ast/block_node.hpp"
#include "conditional_node.hpp"

namespace cmsl::ast {
conditional_node::conditional_node(token_t open_paren,
                                   std::unique_ptr<ast_node> condition,
                                   token_t close_paren,
                                   std::unique_ptr<block_node> body)
  : m_open_paren{ open_paren }
  , m_condition{ std::move(condition) }
  , m_close_paren{ close_paren }
  , m_body{ std::move(body) }
{
}

conditional_node::~conditional_node()
{
}

conditional_node::token_t conditional_node::open_paren() const
{
  return m_open_paren;
}

ast_node::token_t conditional_node::close_paren() const
{
  return m_close_paren;
}

const ast_node& conditional_node::condition() const
{
  return *m_condition;
}

const block_node& conditional_node::body() const
{
  return *m_body;
}

void conditional_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location conditional_node::begin_location() const
{
  return m_open_paren.src_range().begin;
}

source_location conditional_node::end_location() const
{
  return m_body->end_location();
}
}
