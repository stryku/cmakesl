#pragma once

#include "exec/infix/infix_expression.hpp"

#include "lexer/token/token.hpp"
#include "infix_evaluation_context.hpp"

namespace cmsl
{
    namespace exec
    {
        class function_caller;

        namespace infix
        {
            class function_call_expression : public infix_expression
            {
            public:
                explicit function_call_expression(function_caller& fun_caller, lexer::token::token name, std::vector<std::unique_ptr<infix_expression>> parameter_expressions);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;

            private:
                function_caller& m_fun_caller;
                cmsl::lexer::token::token m_name;
                std::vector<std::unique_ptr<infix_expression>> m_parameter_expressions;
            };
        }
    }
}
