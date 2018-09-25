#pragma once

#include "exec/infix/infix_expression.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            class id_expression : public infix_expression
            {
            public:
                explicit id_expression(lexer::token::token identifer);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;
                void visit(infix_expression_visitor& visitor) const override;

            private:
                lexer::token::token m_identifer;
            };
        }
    }
}