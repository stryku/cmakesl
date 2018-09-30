#include "exec/stmt/infix_statement.hpp"
#include "exec/execution.hpp"
#include "ast/infix_node.hpp"
#include "ast/ast_context.hpp"
#include "exec/onp/infix_to_onp.hpp"
#include "exec/onp/onp_executor.hpp"
#include "exec/infix/infix_expression_builder.hpp"
#include "exec/infix/function_call_evaluator.hpp"
#include "exec/infix/infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/infix/infix_expression.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace stmt
        {
            infix_statement::infix_statement(const ast::infix_node& node, std::unique_ptr<inst::instance>& result)
                : m_node{ node }
                , m_result{ result }
            {}

            void infix_statement::execute(execution& e)
            {
                inst::instances_holder instances{ e };
                infix::infix_evaluation_context ctx { e, instances };
                infix::function_call_evaluator caller{ ctx, e };
                infix::infix_expression_builder builder{ caller, m_node.get_tokens() };

                // todo store built expression
                auto expr = builder.build();

                auto result = expr->evaluate(ctx);
                m_result = instances.gather_ownership(result);


             //   const auto onp_tokens = to_onp(e);
               // auto executor = onp::onp_executor{ onp_tokens, e };
                //auto result = executor.execute();
                //m_result = std::move(result);
            }

            infix_statement::tokens_container_t infix_statement::to_onp(execution& e) const
            {
                const auto& tokens = m_node.get_tokens();
                return onp::infix_to_onp{ tokens, e.get_ast_ctx() }.convert();
            }
        }
    }
}
