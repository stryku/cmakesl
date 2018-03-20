#include <gmock/gmock.h>

#include "lexer/lexer.hpp"

namespace cmsl
{
    namespace test
    {
        namespace lexer
        {
            TEST(Lexer, GetToken_Empty_GetInvalid)
            {
                const auto source = "";
                cmsl::lexer::lexer lex;
                const auto token = lex.get_token(source);
                ASSERT_THAT(token.is_valid(), false);
            }
        }
    }
}