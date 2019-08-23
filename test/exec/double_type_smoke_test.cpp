#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using DoubleTypeSmokeTest = ExecutionSmokeTest;

TEST_F(DoubleTypeSmokeTest, DefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    double d;"
                      "    return int(d);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(DoubleTypeSmokeTest, ExplicitDefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    double d = double();"
                      "    return int(d);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(DoubleTypeSmokeTest, ConstructorWithValue)
{
  const auto source = "int main()"
                      "{"
                      "    double d = double(42.6);"
                      "    return int(d);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorPlus)
{
  const auto source = "int main()"
                      "{"
                      "    return int(40.42 + 1.58);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorUnaryPlusPlus)
{
  const auto source = "int main()"
                      "{"
                      "    double foo = 40.0;"
                      "    ++foo;"
                      "    return int(++foo);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorMinus)
{
  const auto source = "int main()"
                      "{"
                      "    return int(44.123 - 2.123);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorUnaryMinus)
{
  const auto source = "int main()"
                      "{"
                      "    return int(-42.0);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(-42));
}

TEST_F(DoubleTypeSmokeTest, OperatorUnaryMinusMinus)
{
  const auto source = "int main()"
                      "{"
                      "    double foo = 44.0;"
                      "    --foo;"
                      "    return int(--foo);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorStar)
{
  const auto source = "int main()"
                      "{"
                      "    return int(84.0 * 0.5);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorSlash)
{
  const auto source = "int main()"
                      "{"
                      "    return int(21.0 / 0.5);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorEqual)
{
  const auto source = "int main()"
                      "{"
                      "    double d;"
                      "    d = 42.0;"
                      "    return int(d);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorPlusEqual)
{
  const auto source = "int main()"
                      "{"
                      "    double d = 21.7;"
                      "    d += 20.3;"
                      "    return int(d);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorMinusEqual)
{
  const auto source = "int main()"
                      "{"
                      "    double d = 63.7;"
                      "    d -= 21.7;"
                      "    return int(d);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorStarEqual)
{
  const auto source = "int main()"
                      "{"
                      "    double d = 21.0;"
                      "    d *= 2.0;"
                      "    return int(d);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorSlashEqual)
{
  const auto source = "int main()"
                      "{"
                      "    double d = 21.0;"
                      "    d /= 0.5;"
                      "    return int(d);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DoubleTypeSmokeTest, OperatorLess)
{
  const auto source = "int main()"
                      "{"
                      "    return int(43.0 < 44.1);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(DoubleTypeSmokeTest, OperatorLessEqual)
{
  const auto source = "int main()"
                      "{"
                      "    return int(44.0 <= 44.0);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(DoubleTypeSmokeTest, OperatorGreater)
{
  const auto source = "int main()"
                      "{"
                      "    return int(42.1 > 42.0);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(DoubleTypeSmokeTest, OperatorGreaterEqual)
{
  const auto source = "int main()"
                      "{"
                      "    return int(42.0 >= 42.0);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
