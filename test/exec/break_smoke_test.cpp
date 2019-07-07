#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using BreakSmokeTest = ExecutionSmokeTest;

TEST_F(BreakSmokeTest, BreakFromForLoop)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    int value;\n"
                      "    for(int i = 0; i < 100; i += 1 )\n"
                      "    {\n"
                      "        if(i == 42)\n"
                      "        {\n"
                      "            value = i;\n"
                      "            break;\n"
                      "        }\n"
                      "    }\n"
                      "    return value;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(BreakSmokeTest, BreakFromNestedForLoop)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    int value;\n"
                      "    for(int i = 0; i < 42; i += 1)"
                      "    {\n"
                      "        for(int j = 0; j < 10; j += 1)\n"
                      "        {\n"
                      "            if(j == 1)\n"
                      "            {\n"
                      "                value += j;\n"
                      "                break;\n"
                      "            }\n"
                      "        }\n"
                      "    }\n"
                      "    return value;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(BreakSmokeTest, BreakFromWhileLoop)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    int value;\n"
                      "    int i;\n"
                      "    while(i < 100 )\n"
                      "    {\n"
                      "        if(i == 42)\n"
                      "        {\n"
                      "            value = i;\n"
                      "            break;\n"
                      "        }\n"
                      "        i += 1;\n"
                      "    }\n"
                      "    return value;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(BreakSmokeTest, BreakFromNestedWhileLoop)
{
  const auto source = "int main()\n"
                      "{\n"
                      "    int value;\n"
                      "    int i;\n"
                      "    while(i < 42)"
                      "    {\n"
                      "        int j;\n"
                      "        while(j < 10)\n"
                      "        {\n"
                      "            if(j == 1)\n"
                      "            {\n"
                      "                value += j;\n"
                      "                break;\n"
                      "            }\n"
                      "            j += 1;"
                      "        }\n"
                      "        i += 1;"
                      "    }\n"
                      "    return value;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
