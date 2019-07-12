#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using StaticVariablesSmokeTest = ExecutionSmokeTest;

TEST_F(StaticVariablesSmokeTest,
       StaticVariableWithoutInitialization_GetDefaultInitialized)
{
  const auto source = "int foo;\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    return foo;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(StaticVariablesSmokeTest,
       StaticVariableWithInitialization_GetDefaultInitialized)
{
  const auto source = "int foo = 42;\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    return foo;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(StaticVariablesSmokeTest, StaticVariableWithInitializationFromFunction)
{
  const auto source = "int foo()\n"
                      "{\n"
                      "    return 42;\n"
                      "}\n"
                      "\n"
                      "auto bar = foo();\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    return bar;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
