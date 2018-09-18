#pragma once

#include "exec/infix/infix_expression.hpp"
#include "exec/instance/instance_value.hpp"
#include "lexer/token/token.hpp"


#include <memory>

namespace cmsl
{
    namespace exec
    {
        namespace infix
        {
            class binary_operator_expression : public infix_expression
            {
            private:
                using token_type_t = lexer::token::token_type;

            public:
                binary_operator_expression(std::unique_ptr<infix_expression> lhs, lexer::token::token op,  std::unique_ptr<infix_expression> rhs);

                inst::instance* evaluate(infix_evaluation_context& ctx) const override;

            private:
                inst::instance_value_t apply_operator_visitor(inst::instance* lhs, token_type_t  op, inst::instance* rhs) const;


            private:
                std::unique_ptr<infix_expression> m_lhs;
                lexer::token::token m_operator;
                std::unique_ptr<infix_expression> m_rhs;
            };
        }
    }
}



