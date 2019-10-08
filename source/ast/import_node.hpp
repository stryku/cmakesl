#pragma once

#include "ast/ast_node.hpp"

namespace cmsl::ast {

class import_node : public ast_node
{
public:
  explicit import_node(const token_t& import_token, const token_t& file_path,
                       const token_t& semicolon);

  const token_t& import_() const;
  const token_t& file_path() const;

  void visit(ast_node_visitor& visitor) const override;
  source_location begin_location() const override;
  source_location end_location() const override;

private:
  token_t m_import;
  token_t m_file_path;
  token_t m_semicolon;
};
}
