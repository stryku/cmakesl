#include "ast/class_node.hpp"

#include "ast/ast_node_visitor.hpp"

#include <assert.h>

namespace cmsl::ast {
class_node::class_node(std::optional<token_t> export_kw, token_t class_kw,
                       token_t name, token_t open_brace, nodes_t nodes,
                       token_t close_brace, token_t semicolon)
  : m_export_kw{ export_kw }
  , m_class_kw{ class_kw }
  , m_name{ name }
  , m_open_brace{ open_brace }
  , m_nodes{ std::move(nodes) }
  , m_close_brace{ close_brace }
  , m_semicolon{ semicolon }
{
}

class_node::token_t class_node::class_kw() const
{
  return m_class_kw;
}

class_node::token_t class_node::name() const
{
  return m_name;
}

class_node::token_t class_node::open_brace() const
{
  return m_open_brace;
}

class_node::token_t class_node::close_brace() const
{
  return m_close_brace;
}

class_node::token_t class_node::semicolon() const
{
  return m_semicolon;
}

const class_node::nodes_t& class_node::nodes() const
{
  return m_nodes;
}

void class_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location class_node::begin_location() const
{
  if (m_export_kw) {
    return m_export_kw->src_range().begin;
  }

  return m_class_kw.src_range().begin;
}

source_location class_node::end_location() const
{
  return m_semicolon.src_range().end;
}

const std::optional<ast_node::token_t>& class_node::export_() const
{
  return m_export_kw;
}

bool class_node::is_exported() const
{
  return m_export_kw.has_value();
}
}
