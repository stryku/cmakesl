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
                struct Lex_OneChar : public testing::Test, testing::WithParamInterface<TokenTestState>
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
                    TokenTestState{ ".", token_type_t::dot },
                    TokenTestState{ "{", token_type_t::open_brace },
                    TokenTestState{ "}", token_type_t::close_brace },
                    TokenTestState{ "[", token_type_t::open_square },
                    TokenTestState{ "]", token_type_t::close_square },
                    TokenTestState{ "(", token_type_t::open_paren },
                    TokenTestState{ ")", token_type_t::close_paren }
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

            namespace operators
            {
                struct Lex_Operator : public testing::Test, testing::WithParamInterface<TokenTestState>
                {};

                TEST_P(Lex_Operator, GetOperator)
                {
                    const auto state = GetParam();
                    cmsl::lexer::lexer lex{ state.source };
                    const auto tokens = lex.lex();
                    ASSERT_THAT(tokens.size(), 1u);
                    ASSERT_THAT(tokens.front().get_type(), state.expected_token);
                }

                const auto values = testing::Values(
                    TokenTestState{ "=", token_type_t::equal },
                    TokenTestState{ "==", token_type_t::equalequal },
                    TokenTestState{ "-", token_type_t::minus },
                    TokenTestState{ "--", token_type_t::minusminus },
                    TokenTestState{ "-=", token_type_t::minusequal },
                    TokenTestState{ "+", token_type_t::plus },
                    TokenTestState{ "++", token_type_t::plusplus },
                    TokenTestState{ "+=", token_type_t::plusequal },
                    TokenTestState{ "&", token_type_t::amp },
                    TokenTestState{ "&&", token_type_t::ampamp },
                    TokenTestState{ "&=", token_type_t::ampequal },
                    TokenTestState{ "|", token_type_t::pipe },
                    TokenTestState{ "||", token_type_t::pipepipe },
                    TokenTestState{ "|=", token_type_t::pipeequal },
                    TokenTestState{ "/", token_type_t::slash },
                    TokenTestState{ "/=", token_type_t::slashequal },
                    TokenTestState{ "*", token_type_t::star },
                    TokenTestState{ "*=", token_type_t::starequal },
                    TokenTestState{ "%", token_type_t::percent },
                    TokenTestState{ "%=", token_type_t::percentequal },
                    TokenTestState{ "!", token_type_t::exclaim },
                    TokenTestState{ "!=", token_type_t::exclaimequal },
                    TokenTestState{ "^", token_type_t::xor },
                    TokenTestState{ "^=", token_type_t::xorequal }
                );
                INSTANTIATE_TEST_CASE_P(Lexer, Lex_Operator, values);
            }
        }
    }
}
