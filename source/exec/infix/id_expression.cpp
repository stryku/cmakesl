#include "exec/infix/id_expression.hpp"
#include "infix_evaluation_context.hpp"
#include "exec/execution_context.hpp"
#include "exec/context_provider.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            id_expression::id_expression(lexer::token::token identifer)
                : m_identifer{ identifer }
            {}

            inst::instance *id_expression::evaluate(infix_evaluation_context &ctx) const
            {
                auto& exec_ctx = ctx.m_ctx_provider.get_exec_ctx();
                return exec_ctx.get_variable(m_identifer.str());
            }
        }
    }
}
