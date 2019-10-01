#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using StaticVariablesSmokeTest = ExecutionSmokeTest;

TEST_F(StaticVariablesSmokeTest,
       StaticVariableWithoutInitialization_GetDefaultInitialized)
{
  const auto source = "int foo;"
                      ""
                      "int main()"
                      "{"
                      "    return foo;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(StaticVariablesSmokeTest,
       StaticVariableWithInitialization_GetDefaultInitialized)
{
  const auto source = "int foo = 42;"
                      ""
                      "int main()"
                      "{"
                      "    return foo;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(StaticVariablesSmokeTest, StaticVariableWithInitializationFromFunction)
{
  const auto source = "int foo()"
                      "{"
                      "    return 42;"
                      "}"
                      ""
                      "auto bar = foo();"
                      ""
                      "int main()"
                      "{"
                      "    return bar;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
