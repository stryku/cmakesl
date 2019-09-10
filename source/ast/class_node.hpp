#pragma once

#include "ast/ast_node.hpp"
#include "common/string.hpp"
#include "lexer/token.hpp"

#include <algorithm>
#include <memory>

namespace cmsl::ast {
class class_node : public ast_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<ast_node>>;

  class_node(std::optional<token_t> export_kw, token_t class_kw, token_t name,
             token_t open_brace, nodes_t nodes, token_t close_brace,
             token_t semicolon);

  const std::optional<token_t>& export_() const;
  token_t class_kw() const;
  token_t name() const;
  token_t open_brace() const;
  token_t close_brace() const;
  token_t semicolon() const;
  const nodes_t& nodes() const;

  bool is_exported() const override;
  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  std::optional<token_t> m_export_kw;
  token_t m_class_kw;
  token_t m_name;
  token_t m_open_brace;
  nodes_t m_nodes;
  token_t m_close_brace;
  token_t m_semicolon;
};
}
