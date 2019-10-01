#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using WhileLoopSmokeTest = ExecutionSmokeTest;

TEST_F(WhileLoopSmokeTest, SimpleWhileLoop)
{
  const auto source = "int main()"
                      "{"
                      "    auto counter = 0;"
                      "    while(counter < 42)"
                      "    {"
                      "        counter += 1;"
                      "    }"
                      "    return counter;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
