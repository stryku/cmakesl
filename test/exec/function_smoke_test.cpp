#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using FunctionSmokeTest = ExecutionSmokeTest;

TEST_F(FunctionSmokeTest, Recursion)
{
  // The n_1 and n_2 are to ensure that local variables have their own scope.
  const auto source = "int fib(int n)"
                      "{"
                      "    if(n == 0)"
                      "    {"
                      "        return 0;"
                      "    }"
                      "    if(n == 1)"
                      "    {"
                      "        return 1;"
                      "    }"
                      ""
                      "    auto n_1 = fib(n - 1);"
                      "    auto n_2 = fib(n - 2);"
                      ""
                      "    return n_1 + n_2;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return fib(9) + 8;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
