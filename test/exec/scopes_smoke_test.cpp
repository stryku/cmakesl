#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ScopesSmokeTest = ExecutionSmokeTest;

TEST_F(ScopesSmokeTest, SingleScope)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    int foo = 42;\n"
                      "    {\n"
                      "        int foo;\n"
                      "        foo = 0;\n"
                      "    }\n"
                      "    return foo;\n"
                      "}";
  const auto result = m_executor.execute(source);
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
                      "int main()\n"
                      "{\n"
                      "    int foo;"
                      "    return fun(foo);\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
