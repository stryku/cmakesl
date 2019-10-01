#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ScopesSmokeTest = ExecutionSmokeTest;

TEST_F(ScopesSmokeTest, SingleScope)
{
  const auto source = "int main()"
                      "{"
                      "    int foo = 42;"
                      "    {"
                      "        int foo;"
                      "        foo = 0;"
                      "    }"
                      "    return foo;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ScopesSmokeTest, FunctionScope)
{
  const auto source = "int fun(int foo)"
                      "{"
                      "    foo = 24;"
                      "    {"
                      "        int foo = 42;"
                      "        return foo;"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    int foo;"
                      "    return fun(foo);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
