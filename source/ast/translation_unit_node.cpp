#include "ast/translation_unit_node.hpp"
#include "ast/ast_node_visitor.hpp"

namespace cmsl::ast {
translation_unit_node::translation_unit_node(nodes_t nodes)
  : m_nodes{ std::move(nodes) }
{
}

const translation_unit_node::nodes_t& translation_unit_node::nodes() const
{
  return m_nodes;
}

void translation_unit_node::visit(ast_node_visitor& visitor) const
{
  visitor.visit(*this);
}

source_location translation_unit_node::begin_location() const
{
  return (!m_nodes.empty()) ? m_nodes.front()->begin_location()
                            : source_location{};
}

source_location translation_unit_node::end_location() const
{
  return (!m_nodes.empty()) ? m_nodes.front()->end_location()
                            : source_location{};
}
}
