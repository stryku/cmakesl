#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ForLoopSmokeTest = ExecutionSmokeTest;

TEST_F(ForLoopSmokeTest, SimpleIncrement)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    int i;\n"
                      "    for(; i < 42; i += 1 ) {}\n"
                      "    return i;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ForLoopSmokeTest, IncrementInitializedValue)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    int value;\n"
                      "    for(int i = 24; i < 43; i += 1 )"
                      "    {\n"
                      "        value = i;\n"
                      "    }\n"
                      "    return value;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
