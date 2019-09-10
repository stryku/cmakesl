#include "ast/import_node.hpp"

#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast {

import_node::import_node(const ast_node::token_t& import_token,
                         const ast_node::token_t& file_path)
  : m_import{ import_token }
  , m_file_path{ file_path }
{
}

const ast_node::token_t& import_node::import_() const
{
  return m_import;
}

const ast_node::token_t& import_node::file_path() const
{
  return m_file_path;
}

void import_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location import_node::begin_location() const
{
  return m_import.src_range().begin;
}

source_location import_node::end_location() const
{
  return m_file_path.src_range().end;
}
}
