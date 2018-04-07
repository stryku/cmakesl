#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"
#include "errors/errors_observer.hpp"

#include <gmock/gmock.h>

#include <sstream>

namespace cmsl
{
    namespace test
    {
        namespace lexer_error
        {
            using lexer_t = cmsl::lexer::lexer;
            using token_type_t = cmsl::lexer::token::token_type;

            struct TokenTestState
            {
                std::string source;
                token_type_t expected_token;

                friend std::ostream& operator<<(std::ostream& out, const TokenTestState& state)
                {
                    out << "{source}: " << state.source << " ------------{expected_token}: " << static_cast<int>(state.expected_token);
                    return out;
                }
            };

            TEST(LexerErrorTest, Empty_NoError)
            {
                const auto source = "";
                cmsl::lexer::lexer lex{ source };
                const auto tokens = lex.lex();
                ASSERT_THAT(tokens.empty(), true);
            }
        }
    }
}
