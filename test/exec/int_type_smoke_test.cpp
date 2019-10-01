#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using IntTypeSmokeTest = ExecutionSmokeTest;

TEST_F(IntTypeSmokeTest, DefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    int i;"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(IntTypeSmokeTest, ExplicitDefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    int i = int();"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(IntTypeSmokeTest, ExplicitConstructorWithValue)
{
  const auto source = "int main()"
                      "{"
                      "    int i = int(42);"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, ValueInitialize)
{
  const auto source = "int main()"
                      "{"
                      "    int i = 42;"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorEqual)
{
  const auto source = "int main()"
                      "{"
                      "    int i;"
                      "    i = 42;"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorPlus)
{
  const auto source = "int main()"
                      "{"
                      "    return 40 + 2;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorPlusPlus)
{
  const auto source = "int main()"
                      "{"
                      "    int foo = 40;"
                      "    ++foo;"
                      "    return ++foo;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorMinus)
{
  const auto source = "int main()"
                      "{"
                      "    return 100 - 58;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorUnaryMinus)
{
  const auto source = "int main()"
                      "{"
                      "    return -42;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(-42));
}

TEST_F(IntTypeSmokeTest, OperatorUnaryMinusAsFactor)
{
  const auto source = "int main()"
                      "{"
                      "    return 100 + -58;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorMinusMinus)
{
  const auto source = "int main()"
                      "{"
                      "    int foo = 44;"
                      "    --foo;"
                      "    return --foo;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorStar)
{
  const auto source = "int main()"
                      "{"
                      "    return 11 * 4;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(44));
}

TEST_F(IntTypeSmokeTest, OperatorSlash)
{
  const auto source = "int main()"
                      "{"
                      "    return 47 / 4;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(11));
}

TEST_F(IntTypeSmokeTest, OperatorPlusEqual)
{
  const auto source = "int main()"
                      "{"
                      "    int i = 40;"
                      "    i += 2;"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorMinusEqual)
{
  const auto source = "int main()"
                      "{"
                      "    int i = 44;"
                      "    i -= 2;"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(IntTypeSmokeTest, OperatorStarEqual)
{
  const auto source = "int main()"
                      "{"
                      "    int i = 11;"
                      "    i *= 4;"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(44));
}

TEST_F(IntTypeSmokeTest, OperatorSlashEqual)
{
  const auto source = "int main()"
                      "{"
                      "    int i = 47;"
                      "    i /= 4;"
                      "    return i;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(11));
}

TEST_F(IntTypeSmokeTest, OperatorLess)
{
  const auto source = "int main()"
                      "{"
                      "    return int(44 < 55);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(IntTypeSmokeTest, OperatorLessEqual)
{
  const auto source = "int main()"
                      "{"
                      "    return int(44 <= 44);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(IntTypeSmokeTest, OperatorGreater)
{
  const auto source = "int main()"
                      "{"
                      "    return int(44 > 33);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(IntTypeSmokeTest, OperatorGreaterEqual)
{
  const auto source = "int main()"
                      "{"
                      "    return int(44 >= 44);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(IntTypeSmokeTest, OperatorEqualEqual)
{
  const auto source = "int main()"
                      "{"
                      "    return int(44 == 44);"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
