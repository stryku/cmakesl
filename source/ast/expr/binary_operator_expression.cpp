#include "ast\expr\binary_operator_expression.hpp"

namespace cmsl
{
    namespace ast
    {
        binary_operator_expression::binary_operator_expression(expression& left,
                                                               expression& right,
                                                               lexer::token::token tok)
            : m_left{ left }
            , m_right{ right }
            , m_token{ tok }
        {}

        binary_operator_expression::result_t binary_operator_expression::evaluate()
        {
            const auto left_result = m_left.evaluate();
            const auto right_result = m_right.evaluate();

            switch (m_token.get_type())
            {
                case lexer::token::token::token_type_t::plus: return boost::get<int>(left_result) + boost::get<int>(right_result);
                case lexer::token::token::token_type_t::minus: return boost::get<int>(left_result) - boost::get<int>(right_result);
            }

            return 0;
        }
    }
}
