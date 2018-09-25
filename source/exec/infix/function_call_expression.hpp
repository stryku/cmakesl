#pragma once

#include "exec/infix/call_expression.hpp"

#include "lexer/token/token.hpp"
#include "infix_evaluation_context.hpp"

namespace cmsl
{
    namespace exec
    {
        class function_caller;

        namespace infix
        {
            class function_call_expression : public call_expression
            {
            public:
                using params_t = std::vector<std::unique_ptr<infix_expression>>;

                explicit function_call_expression(function_caller& fun_caller, lexer::token::token name, params_t parameter_expressions);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;
                void visit(infix_expression_visitor& visitor) const override;
            };
        }
    }
}
