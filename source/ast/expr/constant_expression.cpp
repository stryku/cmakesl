#include "ast\expr\constant_expression.hpp"

namespace cmsl
{
    namespace ast
    {
        constant_expression::constant_expression(lexer::token::token tok)
            : m_token{ tok }
        {}

        constant_expression::result_t constant_expression::evaluate()
        {
            return std::stoi(m_token.str().to_string());
        }
    }
}
