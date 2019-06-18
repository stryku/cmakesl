#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using IfElseSmokeTest = ExecutionSmokeTest;

TEST_F(IfElseSmokeTest, If_ExecuteIfBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = true;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 42;\n"
                      "    }\n"
                      "\n"
                      "    return 0;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, If_DontExecuteIfBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = false;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 0;\n"
                      "    }\n"
                      "\n"
                      "    return 42;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElse_ExecuteIfBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = true;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 42;\n"
                      "    }\n"
                      "    else"
                      "    {"
                      "        return 24;"
                      "    }"
                      "\n"
                      "    return 0;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElse_ExecuteElseBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = false;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 24;\n"
                      "    }\n"
                      "    else\n"
                      "    {"
                      "        return 42;"
                      "    }"
                      "\n"
                      "    return 0;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIf_ExecuteIfBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = true;\n"
                      "    auto flag2 = true;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 42;\n"
                      "    }\n"
                      "    else if(flag2)\n"
                      "    {"
                      "        return 24;"
                      "    }"
                      "\n"
                      "    return 0;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIf_ExecuteElseIfBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = false;\n"
                      "    auto flag2 = true;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 24;\n"
                      "    }\n"
                      "    else if(flag2)\n"
                      "    {"
                      "        return 42;"
                      "    }"
                      "\n"
                      "    return 0;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIf_DontExecuteIfElseIfBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = false;\n"
                      "    auto flag2 = false;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 24;\n"
                      "    }\n"
                      "    else if(flag2)\n"
                      "    {\n"
                      "        return 0;\n"
                      "    }\n"
                      "\n"
                      "    return 42;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

//////////////

TEST_F(IfElseSmokeTest, IfElseIfElse_ExecuteIfBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = true;\n"
                      "    auto flag2 = true;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 42;\n"
                      "    }\n"
                      "    else if(flag2)\n"
                      "    {\n"
                      "        return 24;\n"
                      "    }\n"
                      "    else\n"
                      "    {"
                      "        return 22;\n"
                      "    }\n"
                      "\n"
                      "    return 0;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIfElse_ExecuteElseIfBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = false;\n"
                      "    auto flag2 = true;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 24;\n"
                      "    }\n"
                      "    else if(flag2)\n"
                      "    {\n"
                      "        return 42;\n"
                      "    }\n"
                      "    else\n"
                      "    {"
                      "        return 22;\n"
                      "    }\n"
                      "\n"
                      "    return 0;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IfElseSmokeTest, IfElseIfElse_ExecuteElseBody)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto flag = false;\n"
                      "    auto flag2 = false;\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 24;\n"
                      "    }\n"
                      "    else if(flag2)\n"
                      "    {\n"
                      "        return 0;\n"
                      "    }\n"
                      "    else\n"
                      "    {"
                      "        return 42;\n"
                      "    }\n"
                      "\n"
                      "    return 22;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
