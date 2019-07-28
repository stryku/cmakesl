#include "ast/user_function_node.hpp"

#include "ast/ast_node_visitor.hpp"
#include "ast/block_node.hpp"

namespace cmsl::ast {
user_function_node::user_function_node(type_representation return_type,
                                       token_t name, token_t open_paren,
                                       params_t params, token_t close_paren,
                                       std::unique_ptr<block_node> body)
  : m_return_type{ std::move(return_type) }
  , m_name{ name }
  , m_open_paren{ open_paren }
  , m_params{ std::move(params) }
  , m_close_paren{ close_paren }
  , m_body{ std::move(body) }
{
}

const type_representation& user_function_node::return_type_representation()
  const
{
  return m_return_type;
}

user_function_node::token_t user_function_node::name() const
{
  return m_name;
}

user_function_node::token_t user_function_node::open_paren() const
{
  return m_open_paren;
}

user_function_node::token_t user_function_node::close_paren() const
{
  return m_close_paren;
}

const user_function_node::params_t& user_function_node::param_declarations()
  const
{
  return m_params;
}

const block_node& user_function_node::body() const
{
  return *m_body;
}

void user_function_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location user_function_node::begin_location() const
{
  return m_return_type.src_range().begin;
}

source_location user_function_node::end_location() const
{
  return m_body->end_location();
}
}
