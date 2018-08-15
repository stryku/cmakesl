#include "exec/stmt/declaration_statement.hpp"

#include "ast/declaration_node.hpp"
#include "exec/exec.hpp"
#include "exec/stmt/infix_statement.hpp"

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
                const auto& expr = m_node.get_expression();
                int infix_result;
                auto infix = infix_statement{ expr, infix_result };
                infix.execute(e);
                auto& ctx = e.get_exec_ctx();
                ctx.add_variable(m_node.get_name(), infix_result);
            }
        }
    }
}
