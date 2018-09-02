#include "ast/return_node.hpp"

#include "exec/source_executor.hpp"

namespace cmsl
{
    namespace ast
    {
        return_node::return_node(std::unique_ptr<infix_node> expr)
            : ast_node(ast_node_type::ret)
            , m_expression{ std::move(expr) }
        {}

        infix_node& return_node::get_expression()
        {
            return *m_expression;
        }
    }
}
