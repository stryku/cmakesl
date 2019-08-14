#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using BoolTypeSmokeTest = ExecutionSmokeTest;

TEST_F(BoolTypeSmokeTest, DefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    bool b;"
                      "    return int(b);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(BoolTypeSmokeTest, ExplicitDefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    bool b = bool();"
                      "    return int(b);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(BoolTypeSmokeTest, ExplicitConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    bool b = bool(true);"
                      "    bool b2 = bool(false);"
                      "    return int(b);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(BoolTypeSmokeTest, ConstructorFromInt)
{
  const auto source = "int main()"
                      "{"
                      "    int foo = 42;"
                      "    bool b = bool(foo);"
                      "    return int(b);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(BoolTypeSmokeTest, ConstructorFromIntZero)
{
  const auto source = "int main()"
                      "{"
                      "    int foo = 0;"
                      "    bool b = bool(foo);"
                      "    return int(b);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(BoolTypeSmokeTest, OperatorEqual)
{
  const auto source = "int main()"
                      "{"
                      "    bool b;"
                      "    b = true;"
                      "    return int(b);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(BoolTypeSmokeTest, OperatorPipePipe)
{
  const auto source = "int main()"
                      "{"
                      "    return int(false || true);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(BoolTypeSmokeTest, OperatorAmpAmp)
{
  const auto source = "int main()"
                      "{"
                      "    return int(true && false);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}
}
