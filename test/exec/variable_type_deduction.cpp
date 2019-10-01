#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using VariableTypeDeductionTest = ExecutionSmokeTest;

TEST_F(VariableTypeDeductionTest, InitByCopy)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = 42;"
                      "    return foo;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(VariableTypeDeductionTest, InitReference)
{
  const auto source = "int main()"
                      "{"
                      "    auto foo = 24;"
                      "    auto& bar = foo;"
                      "    bar = 42;"
                      "    return int(foo == 42 && bar == 42);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
