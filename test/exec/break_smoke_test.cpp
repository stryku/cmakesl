#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using BreakSmokeTest = ExecutionSmokeTest;

TEST_F(BreakSmokeTest, BreakFromForLoop)
{
  const auto source = "int main()"
                      "{"
                      "    int value;"
                      "    for(int i = 0; i < 100; i += 1 )"
                      "    {"
                      "        if(i == 42)"
                      "        {"
                      "            value = i;"
                      "            break;"
                      "        }"
                      "    }"
                      "    return value;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(BreakSmokeTest, BreakFromNestedForLoop)
{
  const auto source = "int main()"
                      "{"
                      "    int value;"
                      "    for(int i = 0; i < 42; i += 1)"
                      "    {"
                      "        for(int j = 0; j < 10; j += 1)"
                      "        {"
                      "            if(j == 1)"
                      "            {"
                      "                value += j;"
                      "                break;"
                      "            }"
                      "        }"
                      "    }"
                      "    return value;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(BreakSmokeTest, BreakFromWhileLoop)
{
  const auto source = "int main()"
                      "{"
                      "    int value;"
                      "    int i;"
                      "    while(i < 100 )"
                      "    {"
                      "        if(i == 42)"
                      "        {"
                      "            value = i;"
                      "            break;"
                      "        }"
                      "        i += 1;"
                      "    }"
                      "    return value;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(BreakSmokeTest, BreakFromNestedWhileLoop)
{
  const auto source = "int main()"
                      "{"
                      "    int value;"
                      "    int i;"
                      "    while(i < 42)"
                      "    {"
                      "        int j;"
                      "        while(j < 10)"
                      "        {"
                      "            if(j == 1)"
                      "            {"
                      "                value += j;"
                      "                break;"
                      "            }"
                      "            j += 1;"
                      "        }"
                      "        i += 1;"
                      "    }"
                      "    return value;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
