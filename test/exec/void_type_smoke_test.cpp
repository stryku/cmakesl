#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using VoidTypeSmokeTest = ExecutionSmokeTest;

TEST_F(VoidTypeSmokeTest, EmptyVoidFunction)
{
  const auto source = "void foo()\n"
                      "{}\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    foo();\n"
                      "    return 42;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(VoidTypeSmokeTest, FunctionReturningVoid)
{
  const auto source = "void foo(int& i)\n"
                      "{\n"
                      "    i = 42;\n"
                      "}\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    int i;\n"
                      "    foo( i );\n"
                      "    return i;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
