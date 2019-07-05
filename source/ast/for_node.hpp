#pragma once

#include "ast/ast_node.hpp"

#include <memory>

namespace cmsl::ast {

class block_node;

class for_node : public ast_node
{
public:
  explicit for_node(token_t for_kw, token_t open_paren,
                    std::unique_ptr<ast_node> init, token_t init_semicolon,
                    std::unique_ptr<ast_node> condition,
                    token_t condition_semicolon,
                    std::unique_ptr<ast_node> iteration,
                    ast_node::token_t close_paren,
                    std::unique_ptr<block_node> body);
  ~for_node();

  token_t for_kw() const;
  token_t open_paren() const;
  const ast_node* init() const;
  token_t init_semicolon() const;
  const ast_node* condition() const;
  token_t condition_semicolon() const;
  const ast_node* iteration() const;
  token_t close_paren() const;
  const block_node& body() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  token_t m_for_kw;
  token_t m_open_paren;
  std::unique_ptr<ast_node> m_init;
  token_t m_init_semicolon;
  std::unique_ptr<ast_node> m_condition;
  token_t m_condition_semicolon;
  std::unique_ptr<ast_node> m_iteration;
  token_t m_close_paren;
  std::unique_ptr<block_node> m_body;
};

}
