#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using WhileLoopSmokeTest = ExecutionSmokeTest;

TEST_F(WhileLoopSmokeTest, SimpleWhileLoop)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    auto counter = 0;\n"
                      "    while(counter < 42)"
                      "    {"
                      "        counter += 1;"
                      "    }"
                      "    return counter;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
