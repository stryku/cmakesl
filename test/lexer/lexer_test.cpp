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

            TEST(Lexer, GetToken_Empty_GetEmpty)
            {
                const auto source = "";
                cmsl::lexer::lexer lex;
                const auto tokens = lex.lex(source);
                ASSERT_THAT(tokens.empty(), true);
            }

            namespace integer
            {
                struct GetToken : public testing::Test, testing::WithParamInterface<std::string>
                {};

                TEST_P(GetToken, Digit_GetInteger)
                {
                    const auto source = GetParam();
                    cmsl::lexer::lexer lex;
                    const auto tokens = lex.lex(source);
                    ASSERT_THAT(tokens.size(), 1u);
                    ASSERT_THAT(tokens.front().get_type(), token_type_t::integer);
                }

                const auto values = testing::Values("0", "1", "2", "3", "4", "5", "6", "7", "8", "9");
                INSTANTIATE_TEST_CASE_P(Lexer, GetToken, values);
            }
        }
    }
}