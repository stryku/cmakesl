#include "ast/for_node.hpp"
#include "ast/ast_node_visitor.hpp"
#include "ast/block_node.hpp"

namespace cmsl::ast {
for_node::for_node(ast_node::token_t for_kw, ast_node::token_t open_paren,
                   std::unique_ptr<ast_node> init,
                   ast_node::token_t init_semicolon,
                   std::unique_ptr<ast_node> condition,
                   ast_node::token_t condition_semicolon,
                   std::unique_ptr<ast_node> iteration,
                   ast_node::token_t close_paren,
                   std::unique_ptr<block_node> body)
  : m_for_kw{ for_kw }
  , m_open_paren{ open_paren }
  , m_init{ std::move(init) }
  , m_init_semicolon{ init_semicolon }
  , m_condition{ std::move(condition) }
  , m_condition_semicolon{ condition_semicolon }
  , m_iteration{ std::move(iteration) }
  , m_close_paren{ close_paren }
  , m_body{ std::move(body) }
{
}

for_node::~for_node() = default;

ast_node::token_t for_node::for_kw() const
{
  return m_for_kw;
}

ast_node::token_t for_node::open_paren() const
{
  return m_open_paren;
}

const ast_node* for_node::init() const
{
  return m_init.get();
}

ast_node::token_t for_node::init_semicolon() const
{
  return m_init_semicolon;
}

const ast_node* for_node::condition() const
{
  return m_condition.get();
}

ast_node::token_t for_node::condition_semicolon() const
{
  return m_condition_semicolon;
}

const ast_node* for_node::iteration() const
{
  return m_iteration.get();
}

ast_node::token_t for_node::close_paren() const
{
  return m_close_paren;
}

const block_node& for_node::body() const
{
  return *m_body;
}

void for_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location for_node::begin_location() const
{
  return m_for_kw.src_range().begin;
}

source_location for_node::end_location() const
{
  return m_body->end_location();
}
}
