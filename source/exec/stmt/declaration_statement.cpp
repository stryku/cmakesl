#include "exec/stmt/declaration_statement.hpp"

#include "ast/declaration_node.hpp"
#include "exec/source_executor.hpp"
#include "exec/stmt/infix_statement.hpp"
#include "exec/instance/instance_factory.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            declaration_statement::declaration_statement(ast::declaration_node& node)
                : m_node{ node }
            {}

            void declaration_statement::execute(source_executor& e)
            {
                const auto expr = m_node.get_expression();
                std::unique_ptr<inst::instance> instance;
                auto factory = inst::instance_factory{ e.get_ast_ctx(), e.get_exec_ctx() };

                if(expr)
                {
                    inst::instance_value_t infix_result;
                    auto infix = infix_statement{ *expr, infix_result };
                    infix.execute(e);
                    instance = factory.create(infix_result);
                }
                else
                {
                    instance = factory.create(m_node.get_declared_type());
                }

                auto& ctx = e.get_exec_ctx();
                ctx.add_variable(m_node.get_name(), std::move(instance));
            }
        }
    }
}
