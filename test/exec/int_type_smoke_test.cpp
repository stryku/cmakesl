#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            using ::testing::Eq;
            using IntTypeSmokeTest = ExecutionSmokeTest;

            TEST_F(IntTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i;\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(0));
            }

            TEST_F(IntTypeSmokeTest, ExplicitDefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i = int();\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(0));
            }

            TEST_F(IntTypeSmokeTest, ExplicitConstructorWithValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i = int(42);\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, ValueInitialize)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i = 42;\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, OperatorEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i;\n"
                        "    i = 42;"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, OperatorPlus)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return 40 + 2;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, OperatorMinus)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return 100 - 58;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, OperatorStar)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return 11 * 4;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(44));
            }

            TEST_F(IntTypeSmokeTest, OperatorSlash)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return 47 / 4;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(11));
            }

            TEST_F(IntTypeSmokeTest, OperatorPlusEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i = 40;\n"
                        "    i += 2;\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, OperatorMinusEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i = 44;\n"
                        "    i -= 2;\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, OperatorStarEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i = 11;\n"
                        "    i *= 4;\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(44));
            }

            TEST_F(IntTypeSmokeTest, OperatorSlashEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i = 47;\n"
                        "    i /= 4;\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(11));
            }

            TEST_F(IntTypeSmokeTest, OperatorLess)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(44 < 55);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(IntTypeSmokeTest, OperatorLessEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(44 <= 44);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(IntTypeSmokeTest, OperatorGreater)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(44 > 33);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(IntTypeSmokeTest, OperatorGreaterEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(44 >= 44);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(IntTypeSmokeTest, OperatorEqualEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(44 == 44);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }
        }
    }
}
