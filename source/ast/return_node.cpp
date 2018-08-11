#include "ast/return_node.hpp"

#include "exec/exec.hpp"

namespace cmsl
{
    namespace ast
    {
        return_node::return_node(std::unique_ptr<infix_node> expr)
            : ast_node(ast_node_type::ret)
            , m_expression{ std::move(expr) }
        {}

        void return_node::execute(exec::executor& executor)
        {
            auto val = m_expression->evaluate();
            executor.set_function_return_value(val);
        }
    }
}
