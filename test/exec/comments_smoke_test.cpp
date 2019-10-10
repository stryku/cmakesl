#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using CommentsSmokeTest = ExecutionSmokeTest;

TEST_F(CommentsSmokeTest, OneLineComment)
{
  const auto source = "int main()"
                      "{"
                      "    // return 0;\n"
                      "    return 42;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CommentsSmokeTest, MultilineComment)
{
  const auto source = "int main()"
                      "{\n"
                      "    /* \nreturn\n 0;\n */\n"
                      "    return 42;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CommentsSmokeTest, MultilineCommentInSingleLine)
{
  const auto source = "int main()"
                      "{\n"
                      "    /* return 0; */\n"
                      "    return 42;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CommentsSmokeTest, MultilineCommentInMiddleOfExpression)
{
  const auto source = "int main()"
                      "{\n"
                      "    return 20 + /* 999 */ 22;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
