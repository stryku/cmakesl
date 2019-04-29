#include "errors/errors_observer.hpp"
#include "errors_observer_mock/errors_observer_mock.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"

#include <gmock/gmock.h>

#include <sstream>

using namespace testing;

namespace cmsl::test::lexer_error {
using lexer_t = cmsl::lexer::lexer;
using token_type_t = cmsl::lexer::token_type;

TEST(LexerErrorTest, Empty_NoError)
{
  testing::StrictMock<errors::test::errors_observer_mock> err_observer_mock;
  errors::errors_observer err_observer;
  const auto source = "";
  cmsl::lexer::lexer lex{ err_observer, cmsl::source_view{ source } };
  (void)lex.lex();
}

namespace eof {
struct StringNotEndedBeforeEOF
  : public Test
  , WithParamInterface<std::string>
{
};

TEST_P(StringNotEndedBeforeEOF, NotifyError)
{
  errors::test::errors_observer_mock err_observer_mock;
  errors::errors_observer err_observer;

  EXPECT_CALL(err_observer_mock, notify_error(_)).Times(1);

  const auto source = GetParam();
  auto lex = lexer_t{ err_observer, cmsl::source_view{ source } };
  (void)lex.lex();
}

const auto values = Values("\"", "abc\"", "\"abc", "\"abc\\\"", "\"abc''");
INSTANTIATE_TEST_CASE_P(LexerErrorTest, StringNotEndedBeforeEOF, values);
}
}
