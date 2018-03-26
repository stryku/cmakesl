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

            

            TEST(Lexer_Lex, Empty_GetEmpty)
            {
                const auto source = "";
                cmsl::lexer::lexer lex{ source };
                const auto tokens = lex.lex();
                ASSERT_THAT(tokens.empty(), true);
            }

            namespace integer
            {
                struct Lex_Digit : public testing::Test, testing::WithParamInterface<std::string>
                {};

                TEST_P(Lex_Digit, GetInteger)
                {
                    const auto source = GetParam();
                    cmsl::lexer::lexer lex{ source };
                    const auto tokens = lex.lex();
                    ASSERT_THAT(tokens.size(), 1u);
                    ASSERT_THAT(tokens.front().get_type(), token_type_t::integer);
                }

                const auto values = testing::Values("0", "1", "2", "3", "4", "5", "6", "7", "8", "9");
                INSTANTIATE_TEST_CASE_P(Lexer, Lex_Digit, values);
            }

            namespace real
            {
                struct Lex_Real : public testing::Test, testing::WithParamInterface<std::string>
                {};

                TEST_P(Lex_Real, GetReal)
                {
                    const auto source = GetParam();
                    cmsl::lexer::lexer lex{ source };
                    const auto tokens = lex.lex();
                    ASSERT_THAT(tokens.size(), 1u);
                    ASSERT_THAT(tokens.front().get_type(), token_type_t::real);
                }

                const auto values = testing::Values("1.0", "123.0", "123.012", "000123.123000",
                                                    "1.", "123.",
                                                    ".0", ".012");
                INSTANTIATE_TEST_CASE_P(Lexer, Lex_Real, values);
            }

            namespace dot
            {
                TEST(Lexer_Lex, Dot_GetDot)
                {
                    const auto source = ".";
                    cmsl::lexer::lexer lex{ source };
                    const auto tokens = lex.lex();
                    ASSERT_THAT(tokens.size(), 1u);
                    ASSERT_THAT(tokens.front().get_type(), token_type_t::dot);
                }
            }
        }
    }
}