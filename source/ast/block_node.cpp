#include "ast/block_node.hpp"
#include "ast/ast_node_visitor.hpp"
#include "block_node.hpp"


#include <algorithm>
#include <iterator>

namespace cmsl::ast
{
    block_node::block_node(token_t open_brace, nodes_t nodes, token_t close_brace)
        : m_open_brace{ open_brace }
        , m_nodes{ std::move(nodes) }
        , m_close_brace{ close_brace }
    {}

    const block_node::nodes_t& block_node::nodes() const
    {
        return m_nodes;
    }

    void block_node::visit(ast_node_visitor &visitor) const
    {
        visitor.visit(*this);
    }

    ast_node::token_t block_node::open_brace() const
    {
        return m_open_brace;
    }

    ast_node::token_t block_node::close_brace() const
    {
        return m_close_brace;
    }

    source_location block_node::begin_location() const
    {
        return m_open_brace.src_range().begin;
    }

    source_location block_node::end_location() const
    {
        return m_close_brace.src_range().end;
    }
}
