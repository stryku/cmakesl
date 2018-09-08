#include "exec/stmt/return_statement.hpp"

#include "ast/return_node.hpp"
#include "exec/execution.hpp"
#include "exec/stmt/infix_statement.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            return_statement::return_statement(ast::return_node& node)
                : m_node{ node }
            {}

            void return_statement::execute(execution& e)
            {
                auto& expr = m_node.get_expression();
                std::unique_ptr<inst::instance> infix_result;
                auto infix = infix_statement{ expr, infix_result };
                infix.execute(e);
                e.return_from_function(std::move(infix_result));
            }
        }
    }
}
