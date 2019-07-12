#include "ast/namespace_node.hpp"

#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast {

namespace_node::namespace_node(token_t namespace_kw, names_t names,
                               token_t open_brace, nodes_t nodes,
                               token_t close_brace)
  : m_namespace{ namespace_kw }
  , m_names{ std::move(names) }
  , m_open_brace{ open_brace }
  , m_nodes{ std::move(nodes) }
  , m_close_brace{ close_brace }
{
}

const namespace_node::token_t& namespace_node::namespace_() const
{
  return m_namespace;
}

const namespace_node::names_t& namespace_node::names() const
{
  return m_names;
}

const namespace_node::token_t& namespace_node::open_brace() const
{
  return m_open_brace;
}

const namespace_node::nodes_t& namespace_node::nodes() const
{
  return m_nodes;
}

const namespace_node::token_t& namespace_node::close_brace() const
{
  return m_close_brace;
}

void namespace_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location namespace_node::begin_location() const
{
  return m_namespace.src_range().begin;
}

source_location namespace_node::end_location() const
{
  return m_close_brace.src_range().end;
}
}