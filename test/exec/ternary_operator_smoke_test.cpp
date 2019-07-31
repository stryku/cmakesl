#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using TernaryOperatorSmokeTest = ExecutionSmokeTest;

TEST_F(TernaryOperatorSmokeTest, TrueCondition)
{
  const auto source = "int main()"
                      "{"
                      "    return true ? 42 : 0;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(TernaryOperatorSmokeTest, FalseCondition)
{
  const auto source = "int main()"
                      "{"
                      "    return false ? 0 : 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(TernaryOperatorSmokeTest, ReturnsReferences)
{
  const auto source = "int main()"
                      "{"
                      "    int foo;"
                      "    int bar;"
                      "    int& baz = true ? foo : bar;"
                      "    baz = 42;"
                      "    return foo;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

}
