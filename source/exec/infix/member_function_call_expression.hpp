#pragma once

#include "exec/infix/call_expression.hpp"
#include "lexer/token/token.hpp"

#include <memory>
#include <vector>

namespace cmsl
{
    namespace exec
    {
        class function_caller;

        namespace infix
        {
            class member_function_call_expression : public call_expression
            {
            public:
                explicit member_function_call_expression(function_caller& fun_caller, std::unique_ptr<infix_expression> lhs, lexer::token::token name, params_t params);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;
                void visit(infix_expression_visitor& visitor) const override;

                const infix_expression& get_lhs() const;

            private:
                std::unique_ptr<infix_expression> m_lhs;
            };
        }
    }
}
