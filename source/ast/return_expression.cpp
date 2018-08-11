#include "ast/return_expression.hpp"

#include "exec/exec.hpp"

namespace cmsl
{
    namespace ast
    {
        return_expression::return_expression(std::unique_ptr<infix_expression> expr)
            : ast_node(expression_type::ret)
            , m_expression{ std::move(expr) }
        {}

        void return_expression::execute(exec::executor& executor)
        {
            auto val = m_expression->evaluate();
            executor.set_function_return_value(val);
        }
    }
}
