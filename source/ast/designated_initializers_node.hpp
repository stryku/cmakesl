#pragma once

#include "ast/ast_node.hpp"
#include <memory>

namespace cmsl::ast {

class designated_initializers_node : public ast_node
{
public:
  struct initializer
  {
    token_t dot;
    token_t name;
    token_t equal;
    std::unique_ptr<ast_node> initialization;
    std::optional<token_t> comma;
  };

  using initializers_t = std::vector<initializer>;

  explicit designated_initializers_node(token_t open_brace,
                                        initializers_t initializers,
                                        token_t close_brace)
    : m_open_brace{ open_brace }
    , m_initializers{ std::move(initializers) }
    , m_close_brace{ close_brace }
  {
  }

  const token_t& open_brace() const;
  const initializers_t& initializers() const;
  const token_t& close_brace() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  token_t m_open_brace;
  initializers_t m_initializers;
  token_t m_close_brace;
};
}
