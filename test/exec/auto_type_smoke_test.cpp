#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using AutoTypeSmokeTest = ExecutionSmokeTest;

TEST_F(AutoTypeSmokeTest,
       VariableDeclarationDeduction_SimpleVariableDeclaration)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto val = 42;\n"
                      "    return val;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest, VariableDeclarationDeduction_FromBinaryOperator)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto val = 20 + 22;\n"
                      "    return val;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest, VariableDeclarationDeduction_FromFunctionReturn)
{
  const auto source = "int foo()\n"
                      "{\n"
                      "    return 42;\n"
                      "}\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    auto val = foo();\n"
                      "    return val;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest, FunctionReturnTypeDeduction_FromSimpleValue)
{
  const auto source = "auto foo()\n"
                      "{\n"
                      "    return 42;\n"
                      "}\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    int val = foo();\n"
                      "    return val;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest, FunctionReturnTypeDeduction_FromFunctionReturn)
{
  const auto source = "auto foo()\n"
                      "{\n"
                      "    return 42;\n"
                      "}\n"
                      "\n"
                      "auto bar()\n"
                      "{\n"
                      "    return foo();\n"
                      "}\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    int val = bar();\n"
                      "    return val;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest,
       MultipleReturnStatements_FromFunctionReturnAndSimpleValue)
{
  const auto source = "auto foo()\n"
                      "{\n"
                      "    return 42;\n"
                      "}\n"
                      "\n"
                      "auto bar(bool flag)\n"
                      "{\n"
                      "    if(flag)\n"
                      "    {\n"
                      "        return 42;\n"
                      "    }\n"
                      "    else\n"
                      "    {\n"
                      "        return foo();\n"
                      "    }\n"
                      "}\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    int val = bar(true);\n"
                      "    return val;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(
  AutoTypeSmokeTest,
  VariableDeclarationDeductionWithFunctionReturnTypeDeduction_FromSimpleValue)
{
  const auto source = "auto foo()\n"
                      "{\n"
                      "    return 42;\n"
                      "}\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    auto val = foo();\n"
                      "    return val;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
