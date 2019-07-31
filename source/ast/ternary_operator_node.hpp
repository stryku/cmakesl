#pragma once

#include "ast/ast_node.hpp"

#include <memory>

namespace cmsl::ast {
class ternary_operator_node : public ast_node
{
public:
  explicit ternary_operator_node(std::unique_ptr<ast_node> condition,
                                 token_t question_token,
                                 std::unique_ptr<ast_node> true_,
                                 token_t colon,
                                 std::unique_ptr<ast_node> false_);

  const ast_node& condition() const;
  const token_t& question() const;
  const ast_node& true_() const;
  const token_t& colon() const;
  const ast_node& false_() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  std::unique_ptr<ast_node> m_condition;
  token_t m_question;
  std::unique_ptr<ast_node> m_true;
  token_t m_colon;
  std::unique_ptr<ast_node> m_false;
};
}
