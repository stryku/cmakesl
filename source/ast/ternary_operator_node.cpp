#include "ast/ternary_operator_node.hpp"
#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast {

ternary_operator_node::ternary_operator_node(
  std::unique_ptr<ast_node> condition, token_t question_token,
  std::unique_ptr<ast_node> true_, token_t colon,
  std::unique_ptr<ast_node> false_)
  : m_condition{ std::move(condition) }
  , m_question{ question_token }
  , m_true{ std::move(true_) }
  , m_colon{ colon }
  , m_false{ std::move(false_) }
{
}

const ast_node& ternary_operator_node::condition() const
{
  return *m_condition;
}

const ast_node::token_t& ternary_operator_node::question() const
{
  return m_question;
}

const ast_node& ternary_operator_node::true_() const
{
  return *m_true;
}

const ast_node::token_t& ternary_operator_node::colon() const
{
  return m_colon;
}

const ast_node& ternary_operator_node::false_() const
{
  return *m_false;
}

void ternary_operator_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location ternary_operator_node::begin_location() const
{
  return m_condition->begin_location();
}
source_location ternary_operator_node::end_location() const
{
  return m_false->end_location();
}
}
