#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace lexer
    {
        lexer::tokens_container_t lexer::lex(const std::string)
        {
            return lexer::tokens_container_t{};
        }

        token::token lexer::get_token(const std::string& source) const
        {
            return token::token{};
        }
    }
}
