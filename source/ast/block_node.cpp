#include "ast/block_node.hpp"
#include "ast/ast_node_visitor.hpp"

#include <algorithm>
#include <iterator>

namespace cmsl::ast
{
    block_node::block_node(std::vector<std::unique_ptr<ast_node>> expressions)
        : ast_node{ ast_node_type::block }
        , m_expressions{ std::move(expressions) }
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
}
