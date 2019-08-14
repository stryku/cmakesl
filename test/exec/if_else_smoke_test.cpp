#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using IfElseSmokeTest = ExecutionSmokeTest;

TEST_F(IfElseSmokeTest, If_ExecuteIfBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = true;"
                      "    if(flag)"
                      "    {"
                      "        return 42;"
                      "    }"
                      ""
                      "    return 0;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, If_DontExecuteIfBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = false;"
                      "    if(flag)"
                      "    {"
                      "        return 0;"
                      "    }"
                      ""
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElse_ExecuteIfBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = true;"
                      "    if(flag)"
                      "    {"
                      "        return 42;"
                      "    }"
                      "    else"
                      "    {"
                      "        return 24;"
                      "    }"
                      ""
                      "    return 0;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElse_ExecuteElseBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = false;"
                      "    if(flag)"
                      "    {"
                      "        return 24;"
                      "    }"
                      "    else"
                      "    {"
                      "        return 42;"
                      "    }"
                      ""
                      "    return 0;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIf_ExecuteIfBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = true;"
                      "    auto flag2 = true;"
                      "    if(flag)"
                      "    {"
                      "        return 42;"
                      "    }"
                      "    else if(flag2)"
                      "    {"
                      "        return 24;"
                      "    }"
                      ""
                      "    return 0;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIf_ExecuteElseIfBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = false;"
                      "    auto flag2 = true;"
                      "    if(flag)"
                      "    {"
                      "        return 24;"
                      "    }"
                      "    else if(flag2)"
                      "    {"
                      "        return 42;"
                      "    }"
                      ""
                      "    return 0;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIf_DontExecuteIfElseIfBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = false;"
                      "    auto flag2 = false;"
                      "    if(flag)"
                      "    {"
                      "        return 24;"
                      "    }"
                      "    else if(flag2)"
                      "    {"
                      "        return 0;"
                      "    }"
                      ""
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

//////////////

TEST_F(IfElseSmokeTest, IfElseIfElse_ExecuteIfBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = true;"
                      "    auto flag2 = true;"
                      "    if(flag)"
                      "    {"
                      "        return 42;"
                      "    }"
                      "    else if(flag2)"
                      "    {"
                      "        return 24;"
                      "    }"
                      "    else"
                      "    {"
                      "        return 22;"
                      "    }"
                      ""
                      "    return 0;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIfElse_ExecuteElseIfBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = false;"
                      "    auto flag2 = true;"
                      "    if(flag)"
                      "    {"
                      "        return 24;"
                      "    }"
                      "    else if(flag2)"
                      "    {"
                      "        return 42;"
                      "    }"
                      "    else"
                      "    {"
                      "        return 22;"
                      "    }"
                      ""
                      "    return 0;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIfElse_ExecuteElseBody)
{
  const auto source = "int main()"
                      "{"
                      "    auto flag = false;"
                      "    auto flag2 = false;"
                      "    if(flag)"
                      "    {"
                      "        return 24;"
                      "    }"
                      "    else if(flag2)"
                      "    {"
                      "        return 0;"
                      "    }"
                      "    else"
                      "    {"
                      "        return 42;"
                      "    }"
                      ""
                      "    return 22;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
