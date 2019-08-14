#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using VoidTypeSmokeTest = ExecutionSmokeTest;

TEST_F(VoidTypeSmokeTest, EmptyVoidFunction)
{
  const auto source = "void foo()"
                      "{}"
                      ""
                      "int main()"
                      "{"
                      "    foo();"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(VoidTypeSmokeTest, FunctionReturningVoid)
{
  const auto source = "void foo(int& i)"
                      "{"
                      "    i = 42;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    int i;"
                      "    foo( i );"
                      "    return i;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
