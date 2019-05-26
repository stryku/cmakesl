#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using VariableTypeDeductionTest = ExecutionSmokeTest;

TEST_F(VariableTypeDeductionTest, InitByCopy)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto foo = 42;\n"
                      "    return foo;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(VariableTypeDeductionTest, InitReference)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto foo = 24;\n"
                      "    auto& bar = foo;\n"
                      "    bar = 42;\n"
                      "    return int(foo == 42 && bar == 42);\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
