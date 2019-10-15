#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ForLoopSmokeTest = ExecutionSmokeTest;

TEST_F(ForLoopSmokeTest, SimpleIncrement)
{
  const auto source = "int main()"
                      "{"
                      "    int i;"
                      "    for(; i < 42; i += 1 ) {}"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ForLoopSmokeTest, IncrementInitializedValue)
{
  const auto source = "int main()"
                      "{"
                      "    int value;"
                      "    for(int i = 24; i < 43; i += 1 )"
                      "    {"
                      "        value = i;"
                      "    }"
                      "    return value;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ForLoopSmokeTest, ChecksConditionBeforeFirstIteration)
{
  const auto source = "int main()"
                      "{"
                      "    int value = 42;"
                      "    for(int i = 0; i < 0; i += 1 )"
                      "    {"
                      "        value = 24;"
                      "    }"
                      "    return value;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
