#include "ast/block_node.hpp"
#include "ast/ast_node_visitor.hpp"
#include "block_node.hpp"


#include <algorithm>
#include <iterator>

namespace cmsl::ast
{
    block_node::block_node(token_t open_brace, expressions_t expressions, token_t close_brace)
        : m_open_brace{ open_brace }
        , m_expressions{ std::move(expressions) }
        , m_close_brace{ close_brace }
    {}

    std::vector<const ast_node*> block_node::get_expressions() const
    {
        std::vector<const ast_node*> result;

        std::transform(std::begin(m_expressions), std::end(m_expressions),
                       std::back_inserter(result),
                       [](auto& expr)
                       {
                           return expr.get();
                       });

        return result;
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
}
