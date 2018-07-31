#include "lexer/lexer.hpp"
#include "lexer/token/token.hpp"
#include "errors/errors_observer.hpp"

#include <gmock/gmock.h>

#include <sstream>

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

            errors::errors_observer dummy_err_observer;
            

            auto create_lexer(const std::string& source)
            {
                return cmsl::lexer::lexer{ dummy_err_observer, source };
            }

            TEST(Lexer_Lex, Empty_GetEmpty)
            {
                const auto source = "";
                auto lex = create_lexer(source);
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
                    auto lex = create_lexer(source);
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
                    auto lex = create_lexer(source);
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
                    auto lex = create_lexer(state.source);
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
                    auto lex = create_lexer(source);
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
                    auto lex = create_lexer(state.source);
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

            namespace string
            {
                struct Lex_String : public testing::Test, testing::WithParamInterface<std::string>
                {};

                TEST_P(Lex_String, GetString)
                {
                    const auto source = GetParam();
                    auto lex = create_lexer(source);
                    const auto tokens = lex.lex();
                    ASSERT_THAT(tokens.size(), 1u);
                    ASSERT_THAT(tokens.front().get_type(), token_type_t::string);
                }

                std::string quoted(const std::string& str)
                {
                    std::stringstream ss;
                    ss << std::quoted(str);
                    return ss.str();
                }

                const auto values = testing::Values(quoted(""),
                                                    quoted("123.def"),
                                                    quoted(".123"),
                                                    quoted(" \t\n\r\n"),
                                                    quoted("'a' 'b' 'c'"),
                                                    quoted("foo \"bar\" baz"));
                INSTANTIATE_TEST_CASE_P(Lexer, Lex_String, values);
            }

            namespace whitespaces
            {
                struct Lex_Whitespaces_State
                {
                    std::string source;
                    std::vector<token_type_t> expected_tokens;
                };

                struct Lex_Whitespaces : public testing::Test, testing::WithParamInterface<Lex_Whitespaces_State>
                {};

                TEST_P(Lex_Whitespaces, TokensType)
                {
                    const auto state = GetParam();
                    auto lex = create_lexer(state.source);
                    const auto tokens = lex.lex();

                    ASSERT_THAT(tokens.size(), state.expected_tokens.size());

                    for(auto i = 0u; i < tokens.size(); ++i)
                    {
                        ASSERT_THAT(tokens[i].get_type(), state.expected_tokens[i]);
                    }
                }

                const auto values = testing::Values(Lex_Whitespaces_State{ " \t\n\r\n", {} },
                                                    Lex_Whitespaces_State{ " \t\n123\r\n", { token_type_t::integer } },
                                                    Lex_Whitespaces_State{ "123 \t\n123\r\n", { token_type_t::integer, token_type_t::integer } });
                INSTANTIATE_TEST_CASE_P(Lexer, Lex_Whitespaces, values);
            }

            namespace keywords
            {
                namespace pure
                {
                    struct PureKeywordState
                    {
                        std::string source;
                        token_type_t expected_keyword;
                    };

                    struct PureKeyword : public testing::TestWithParam<PureKeywordState>
                    {};

                    TEST_P(PureKeyword, RecognizeKeyword)
                    {
                        const auto state = GetParam();
                        auto lex = create_lexer(state.source);
                        const auto tokens = lex.lex();

                        ASSERT_THAT(tokens.size(), 1u);
                        ASSERT_THAT(tokens[0].get_type(), state.expected_keyword);
                    }

                    const auto values = testing::Values(PureKeywordState{ "int", token_type_t::t_int },
                                                        PureKeywordState{ "real", token_type_t::t_real });
                    INSTANTIATE_TEST_CASE_P(Lexer, PureKeyword, values);
                }

                namespace polluted
                {
                    struct PollutedKeyword : public testing::TestWithParam<std::string>
                    {};

                    TEST_P(PollutedKeyword, RecognizedIdentifier)
                    {
                        const auto source = GetParam();
                        auto lex = create_lexer(source);
                        const auto tokens = lex.lex();

                        ASSERT_THAT(tokens.size(), 1u);
                        ASSERT_THAT(tokens[0].get_type(), token_type_t::identifier);
                    }

                    const auto values = testing::Values( "int123", "_real");
                    INSTANTIATE_TEST_CASE_P(Lexer, PollutedKeyword, values);
                }
            }
        }
    }
}
