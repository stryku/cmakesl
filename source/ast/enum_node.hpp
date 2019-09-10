#pragma once

#include "ast/ast_node.hpp"

#include <memory>
#include <vector>

namespace cmsl::ast {
class enum_node : public ast_node
{
public:
  explicit enum_node(std::optional<token_t> export_kw, token_t enum_kw,
                     token_t name, token_t open_brace,
                     std::vector<token_t> enumerators, token_t close_brace,
                     token_t semicolon);

  const std::optional<token_t>& export_() const;
  const token_t& enum_kw() const;
  const token_t& name() const;
  const token_t& open_brace() const;
  const std::vector<token_t>& enumerators() const;
  const token_t& close_brace() const;
  const token_t& semicolon() const;

  bool is_exported() const override;
  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  std::optional<token_t> m_export_kw;
  token_t m_enum_kw;
  token_t m_name;
  token_t m_open_brace;
  std::vector<token_t> m_enumerators;
  token_t m_close_brace;
  token_t m_semicolon;
};
}
