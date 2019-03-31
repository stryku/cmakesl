#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            using ::testing::Eq;

            using DoubleTypeSmokeTest = ExecutionSmokeTest;

            TEST_F(DoubleTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    double d;\n"
                        "    return int(d);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(0));
            }

            TEST_F(DoubleTypeSmokeTest, ExplicitDefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    double d = double();\n"
                        "    return int(d);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(0));
            }

            TEST_F(DoubleTypeSmokeTest, ConstructorWithValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    double d = double(42.6);\n"
                        "    return int(d);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorPlus)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(40.42 + 1.58);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorMinus)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(44.123 - 2.123);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorStar)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(84.0 * 0.5);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorSlash)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(21.0 / 0.5);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    double d;\n"
                        "    d = 42.0;"
                        "    return int(d);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorPlusEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    double d = 21.7;\n"
                        "    d += 20.3;\n"
                        "    return int(d);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorMinusEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    double d = 63.7;\n"
                        "    d -= 21.7;\n"
                        "    return int(d);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorStarEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    double d = 21.0;\n"
                        "    d *= 2.0;\n"
                        "    return int(d);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorSlashEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    double d = 21.0;\n"
                        "    d /= 0.5;\n"
                        "    return int(d);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorLess)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(43.0 < 44.1);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorLessEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(44.0 <= 44.0);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorGreater)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(42.1 > 42.0);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(DoubleTypeSmokeTest, OperatorGreaterEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(42.0 >= 42.0);\n"
                        "}";
                const auto result = m_executor.execute(cmsl::source_view{ source });
                EXPECT_THAT(result, Eq(1));
            }
        }
    }
}
