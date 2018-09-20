#pragma once

#include "exec/infix/infix_expression.hpp"
#include "lexer/token/token.hpp"

#include <memory>

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            class class_member_access_expression : public infix_expression
            {
            public:
                explicit class_member_access_expression(std::unique_ptr<infix_expression> lhs, lexer::token::token member_name);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;

            private:
                std::unique_ptr<infix_expression> m_lhs;
                lexer::token::token m_member_name;
            };
        }
    }
}
