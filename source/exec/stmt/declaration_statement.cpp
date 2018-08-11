#include "exec/stmt/declaration_statement.hpp"

#include "ast/declaration_node.hpp"
#include "exec/exec.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            declaration_statement::declaration_statement(ast::declaration_node& node)
                : m_node{ node }
            {}

            void declaration_statement::execute(executor& e)
            {
                auto& expr = m_node.get_expression();
                const auto result = expr.evaluate();
                e.set_function_return_value(result);
            }
        }
    }
}
