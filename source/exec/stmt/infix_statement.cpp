#include "exec/stmt/infix_statement.hpp"
#include "exec/execution.hpp"
#include "ast/infix_node.hpp"
#include "ast/ast_context.hpp"
#include "exec/infix/infix_expression_builder.hpp"
#include "exec/infix/function_call_evaluator.hpp"
#include "exec/infix/infix_evaluation_context.hpp"
#include "exec/instance/instances_holder.hpp"
#include "exec/infix/infix_expression.hpp"
#include "exec/builtin/builtin_function_factory.hpp"

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
                auto instances = std::make_unique<inst::instances_holder>(e);
                infix::infix_evaluation_context ctx { e, *instances };
                builtin::builtin_function_factory factory{e.get_cmake_facade()};
                infix::function_call_evaluator caller{ factory, ctx, e };
                infix::infix_expression_builder builder{ caller, m_node.get_tokens() };

                // todo store built expression
                auto expr = builder.build();

                auto result = expr->evaluate(ctx);
                m_result = result->copy();
            }
        }
    }
}
