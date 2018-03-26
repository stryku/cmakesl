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

            namespace one_char_tokens
            {
                struct OneCharTokenTestState
                {
                    std::string source;
                    token_type_t expected_token;
                };

                struct Lex_OneChar : public testing::Test, testing::WithParamInterface<OneCharTokenTestState>
                {};

                TEST_P(Lex_OneChar, GetOneCharToken)
                {
                    const auto state = GetParam();
                    cmsl::lexer::lexer lex{ state.source };
                    const auto tokens = lex.lex();
                    ASSERT_THAT(tokens.size(), 1u);
                    ASSERT_THAT(tokens.front().get_type(), state.expected_token);
                }

                const auto values = testing::Values(
                    OneCharTokenTestState{ ".", token_type_t::dot },
                    OneCharTokenTestState{ "{", token_type_t::open_brace },
                    OneCharTokenTestState{ "}", token_type_t::close_brace },
                    OneCharTokenTestState{ "[", token_type_t::open_square },
                    OneCharTokenTestState{ "]", token_type_t::close_square },
                    OneCharTokenTestState{ "(", token_type_t::open_paren },
                    OneCharTokenTestState{ ")", token_type_t::close_paren }
                );
                INSTANTIATE_TEST_CASE_P(Lexer, Lex_OneChar, values);
            }

            namespace identifier
            {
                struct Lex_Identifier : public testing::Test, testing::WithParamInterface<std::string>
                {};

                TEST_P(Lex_Identifier, GetIndetifier)
                {
                    const auto source = GetParam();
                    cmsl::lexer::lexer lex{ source };
                    const auto tokens = lex.lex();
                    ASSERT_THAT(tokens.size(), 1u);
                    ASSERT_THAT(tokens.front().get_type(), token_type_t::identifier);
                }

                const auto values = testing::Values("_", "_abc020", "abc_202", "abc010DEF");
                INSTANTIATE_TEST_CASE_P(Lexer, Lex_Identifier, values);
            }
        }
    }
}
