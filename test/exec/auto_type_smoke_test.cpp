#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using AutoTypeSmokeTest = ExecutionSmokeTest;

TEST_F(AutoTypeSmokeTest,
       VariableDeclarationDeduction_SimpleVariableDeclaration)
{
  const auto source = "int main()"
                      "{"
                      "    auto val = 42;"
                      "    return val;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest, VariableDeclarationDeduction_FromBinaryOperator)
{
  const auto source = "int main()"
                      "{"
                      "    auto val = 20 + 22;"
                      "    return val;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest, VariableDeclarationDeduction_FromFunctionReturn)
{
  const auto source = "int foo()"
                      "{"
                      "    return 42;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    auto val = foo();"
                      "    return val;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest, FunctionReturnTypeDeduction_FromSimpleValue)
{
  const auto source = "auto foo()"
                      "{"
                      "    return 42;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    int val = foo();"
                      "    return val;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest, FunctionReturnTypeDeduction_FromFunctionReturn)
{
  const auto source = "auto foo()"
                      "{"
                      "    return 42;"
                      "}"
                      ""
                      "auto bar()"
                      "{"
                      "    return foo();"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    int val = bar();"
                      "    return val;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(AutoTypeSmokeTest,
       MultipleReturnStatements_FromFunctionReturnAndSimpleValue)
{
  const auto source = "auto foo()"
                      "{"
                      "    return 42;"
                      "}"
                      ""
                      "auto bar(bool flag)"
                      "{"
                      "    if(flag)"
                      "    {"
                      "        return 42;"
                      "    }"
                      "    else"
                      "    {"
                      "        return foo();"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    int val = bar(true);"
                      "    return val;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(
  AutoTypeSmokeTest,
  VariableDeclarationDeductionWithFunctionReturnTypeDeduction_FromSimpleValue)
{
  const auto source = "auto foo()"
                      "{"
                      "    return 42;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    auto val = foo();"
                      "    return val;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
