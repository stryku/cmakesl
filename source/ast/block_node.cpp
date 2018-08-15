#include "ast/block_node.hpp"

#include <algorithm>
#include <iterator>

namespace cmsl
{
    namespace ast
    {
        block_node::block_node(std::vector<std::unique_ptr<ast_node>> expressions)
            : ast_node{ ast_node_type::block }
            , m_expressions{ std::move(expressions) }
        {}

        std::vector<ast_node*> block_node::get_expressions() const
        {
            std::vector<ast_node*> result;

            std::transform(std::begin(m_expressions), std::end(m_expressions),
                           std::back_inserter(result),
                           [](auto& expr)
                           {
                               return expr.get();
                           });

            return result;
        }
    }
}
