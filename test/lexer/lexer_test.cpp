#include "errors/errors_observer.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"

#include <gmock/gmock.h>

#include <sstream>

// Todo: change to lexer::test
namespace cmsl::lexer::test {
using lexer_t = cmsl::lexer::lexer;

struct TokenTestState
{
  std::string source;
  token_type expected_token;

  friend std::ostream& operator<<(std::ostream& out,
                                  const TokenTestState& state)
  {
    out << "{source}: " << state.source
        << " {expected_token}: " << static_cast<int>(state.expected_token);
    return out;
  }
};

errors::errors_observer dummy_err_observer;

auto create_lexer(const std::string& source)
{
  return cmsl::lexer::lexer{ dummy_err_observer, cmsl::source_view{ source } };
}

TEST(Lexer_Lex, Empty_GetEmpty)
{
  const auto source = "";
  auto lex = create_lexer(source);
  const auto tokens = lex.lex();
  ASSERT_THAT(tokens.empty(), true);
}

namespace integer {
struct Lex_Digit
  : public testing::Test
  , testing::WithParamInterface<std::string>
{
};

TEST_P(Lex_Digit, GetInteger)
{
  const auto source = GetParam();
  auto lex = create_lexer(source);
  const auto tokens = lex.lex();
  ASSERT_THAT(tokens.size(), 1u);
  ASSERT_THAT(tokens.front().get_type(), token_type::integer);
}

const auto values =
  testing::Values("0", "1", "2", "3", "4", "5", "6", "7", "8", "9");
INSTANTIATE_TEST_CASE_P(Lexer, Lex_Digit, values);
}

namespace real {
struct Lex_Real
  : public testing::Test
  , testing::WithParamInterface<std::string>
{
};

TEST_P(Lex_Real, GetReal)
{
  const auto source = GetParam();
  auto lex = create_lexer(source);
  const auto tokens = lex.lex();
  ASSERT_THAT(tokens.size(), 1u);
  ASSERT_THAT(tokens.front().get_type(), token_type::double_);
}

const auto values = testing::Values("1.0", "123.0", "123.012", "000123.123000",
                                    "1.", "123.", ".0", ".012");
INSTANTIATE_TEST_CASE_P(Lexer, Lex_Real, values);
}

namespace one_char_tokens {
struct Lex_OneChar
  : public testing::Test
  , testing::WithParamInterface<TokenTestState>
{
};

TEST_P(Lex_OneChar, GetOneCharToken)
{
  const auto state = GetParam();
  auto lex = create_lexer(state.source);
  const auto tokens = lex.lex();
  ASSERT_THAT(tokens.size(), 1u);
  ASSERT_THAT(tokens.front().get_type(), state.expected_token);
}

const auto values =
  testing::Values(TokenTestState{ ".", token_type::dot },
                  TokenTestState{ "{", token_type::open_brace },
                  TokenTestState{ "}", token_type::close_brace },
                  TokenTestState{ "[", token_type::open_square },
                  TokenTestState{ "]", token_type::close_square },
                  TokenTestState{ "(", token_type::open_paren },
                  TokenTestState{ ")", token_type::close_paren },
                  TokenTestState{ ";", token_type::semicolon },
                  TokenTestState{ ",", token_type::comma });
INSTANTIATE_TEST_CASE_P(Lexer, Lex_OneChar, values);
}

namespace identifier {
struct Lex_Identifier
  : public testing::Test
  , testing::WithParamInterface<std::string>
{
};

TEST_P(Lex_Identifier, GetIndetifier)
{
  const auto source = GetParam();
  auto lex = create_lexer(source);
  const auto tokens = lex.lex();
  ASSERT_THAT(tokens.size(), 1u);
  ASSERT_THAT(tokens.front().get_type(), token_type::identifier);
}

const auto values = testing::Values("_", "_abc020", "abc_202", "abc010DEF");
INSTANTIATE_TEST_CASE_P(Lexer, Lex_Identifier, values);
}

namespace operators {
struct Lex_Operator
  : public testing::Test
  , testing::WithParamInterface<TokenTestState>
{
};

TEST_P(Lex_Operator, GetOperator)
{
  const auto state = GetParam();
  auto lex = create_lexer(state.source);
  const auto tokens = lex.lex();
  ASSERT_THAT(tokens.size(), 1u);
  ASSERT_THAT(tokens.front().get_type(), state.expected_token);
}

const auto values =
  testing::Values(TokenTestState{ "=", token_type::equal },
                  TokenTestState{ "==", token_type::equalequal },
                  TokenTestState{ "-", token_type::minus },
                  TokenTestState{ "--", token_type::minusminus },
                  TokenTestState{ "-=", token_type::minusequal },
                  TokenTestState{ "+", token_type::plus },
                  TokenTestState{ "++", token_type::plusplus },
                  TokenTestState{ "+=", token_type::plusequal },
                  TokenTestState{ "&", token_type::amp },
                  TokenTestState{ "&&", token_type::ampamp },
                  TokenTestState{ "&=", token_type::ampequal },
                  TokenTestState{ "|", token_type::pipe },
                  TokenTestState{ "||", token_type::pipepipe },
                  TokenTestState{ "|=", token_type::pipeequal },
                  TokenTestState{ "/", token_type::slash },
                  TokenTestState{ "/=", token_type::slashequal },
                  TokenTestState{ "*", token_type::star },
                  TokenTestState{ "*=", token_type::starequal },
                  TokenTestState{ "%", token_type::percent },
                  TokenTestState{ "%=", token_type::percentequal },
                  TokenTestState{ "!", token_type::exclaim },
                  TokenTestState{ "!=", token_type::exclaimequal },
                  TokenTestState{ "^", token_type::xor_ },
                  TokenTestState{ "^=", token_type::xorequal },
                  TokenTestState{ "<", token_type::less },
                  TokenTestState{ "<=", token_type::lessequal },
                  TokenTestState{ ">", token_type::greater },
                  TokenTestState{ ">=", token_type::greaterequal });
INSTANTIATE_TEST_CASE_P(Lexer, Lex_Operator, values);
}

namespace string {
struct Lex_String
  : public testing::Test
  , testing::WithParamInterface<std::string>
{
};

TEST_P(Lex_String, GetString)
{
  const auto source = GetParam();
  auto lex = create_lexer(source);
  const auto tokens = lex.lex();
  ASSERT_THAT(tokens.size(), 1u);
  ASSERT_THAT(tokens.front().get_type(), token_type::string);
}

std::string quoted(const std::string& str)
{
  std::stringstream ss;
  ss << std::quoted(str);
  return ss.str();
}

const auto values = testing::Values(
  quoted(""), quoted("123.def"), quoted(".123"), quoted(" \t\n\r\n"),
  quoted("'a' 'b' 'c'"), quoted("foo \"bar\" baz"));
INSTANTIATE_TEST_CASE_P(Lexer, Lex_String, values);
}

namespace whitespaces {
struct Lex_Whitespaces_State
{
  std::string source;
  std::vector<token_type> expected_tokens;
  std::vector<std::string> expected_tokens_values;
};

struct Lex_Whitespaces
  : public testing::Test
  , testing::WithParamInterface<Lex_Whitespaces_State>
{
};

TEST_P(Lex_Whitespaces, TokensType)
{
  const auto state = GetParam();
  auto lex = create_lexer(state.source);
  const auto tokens = lex.lex();

  ASSERT_THAT(tokens.size(), state.expected_tokens.size());

  for (auto i = 0u; i < tokens.size(); ++i) {
    ASSERT_THAT(tokens[i].get_type(), state.expected_tokens[i]);
  }
}

const auto values = testing::Values(
  Lex_Whitespaces_State{ " \t\n\r\n", {} },
  Lex_Whitespaces_State{ " \t\n123\r\n", { token_type::integer }, { "123" } },
  Lex_Whitespaces_State{ "123 \t\n123\r\n",
                         { token_type::integer, token_type::integer },
                         { "123", "123" } });
INSTANTIATE_TEST_CASE_P(Lexer, Lex_Whitespaces, values);
}

namespace keywords {
namespace pure {
struct PureKeywordState
{
  std::string source;
  token_type expected_keyword;
};

struct PureKeyword : public testing::TestWithParam<PureKeywordState>
{
};

TEST_P(PureKeyword, RecognizeKeyword)
{
  const auto state = GetParam();
  auto lex = create_lexer(state.source);
  const auto tokens = lex.lex();

  ASSERT_THAT(tokens.size(), 1u);
  ASSERT_THAT(tokens[0].get_type(), state.expected_keyword);
}

const auto values =
  testing::Values(PureKeywordState{ "void", token_type::kw_void },
                  PureKeywordState{ "int", token_type::kw_int },
                  PureKeywordState{ "double", token_type::kw_double },
                  PureKeywordState{ "bool", token_type::kw_bool },
                  PureKeywordState{ "true", token_type::kw_true },
                  PureKeywordState{ "false", token_type::kw_false },
                  PureKeywordState{ "string", token_type::kw_string },
                  PureKeywordState{ "version", token_type::kw_version },
                  PureKeywordState{ "list", token_type::kw_list },
                  PureKeywordState{ "library", token_type::kw_library },
                  PureKeywordState{ "executable", token_type::kw_executable },
                  PureKeywordState{ "project", token_type::kw_project },
                  PureKeywordState{ "return", token_type::kw_return },
                  PureKeywordState{ "class", token_type::kw_class },
                  PureKeywordState{ "if", token_type::kw_if },
                  PureKeywordState{ "else", token_type::kw_else },
                  PureKeywordState{ "while", token_type::kw_while },
                  PureKeywordState{ "auto", token_type::kw_auto });
INSTANTIATE_TEST_CASE_P(Lexer, PureKeyword, values);
}

namespace polluted {
struct PollutedKeyword : public testing::TestWithParam<std::string>
{
};

TEST_P(PollutedKeyword, RecognizedIdentifier)
{
  const auto source = GetParam();
  auto lex = create_lexer(source);
  const auto tokens = lex.lex();

  ASSERT_THAT(tokens.size(), 1u);
  ASSERT_THAT(tokens[0].get_type(), token_type::identifier);
}

const auto values = testing::Values("int123", "_double");
INSTANTIATE_TEST_CASE_P(Lexer, PollutedKeyword, values);
}
}
}
