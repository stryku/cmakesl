#include <gmock/gmock.h>

#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"

namespace cmsl
{
    namespace test
    {
        namespace lexer
        {
            using lexer_t = cmsl::lexer::lexer;
            using token_type_t = cmsl::lexer::token::token_type;

            TEST(Lexer, GetToken_Empty_GetInvalid)
            {
                const auto source = "";
                cmsl::lexer::lexer lex;
                const auto token = lex.get_token(source);
                ASSERT_THAT(token.is_valid(), false);
            }

            namespace integer
            {
                struct IntegerTestState
                {
                    std::string source;
                };

                struct GetToken_Digit_GetInteger : public testing::Test, testing::WithParamInterface<IntegerTestState>
                {};

                const auto values = testing::Values(
                    IntegerTestState{ "0" },
                    IntegerTestState{ "1" },
                    IntegerTestState{ "2" },
                    IntegerTestState{ "3" },
                    IntegerTestState{ "4" },
                    IntegerTestState{ "5" },
                    IntegerTestState{ "6" },
                    IntegerTestState{ "7" },
                    IntegerTestState{ "8" },
                    IntegerTestState{ "9" }
                );

                TEST_P(GetToken_Digit_GetInteger, Lexer)
                {
                    const auto state = GetParam();
                    cmsl::lexer::lexer lex;
                    const auto token = lex.get_token(state.source);
                    ASSERT_THAT(token.is_valid(), true);
                    ASSERT_THAT(token.get_type(), token_type_t::integer);
                }
            }
        }
    }
}