#include "ast/return_node.hpp"

namespace cmsl
{
    namespace ast
    {
        return_node::return_node(std::unique_ptr<ast_node> expr)
            : ast_node(ast_node_type::ret)
            , m_expression{ std::move(expr) }
        {}

        ast_node& return_node::get_expression()
        {
            return *m_expression;
        }
    }
}
