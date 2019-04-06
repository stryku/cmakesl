#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test
{
            using ::testing::Eq;

            using BoolTypeSmokeTest = ExecutionSmokeTest;

            TEST_F(BoolTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    bool b;\n"
                        "    return int(b);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(0));
            }

            TEST_F(BoolTypeSmokeTest, ExplicitDefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    bool b = bool();\n"
                        "    return int(b);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(0));
            }

            TEST_F(BoolTypeSmokeTest, ExplicitConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    bool b = bool(true);\n"
                        "    bool b2 = bool(false);\n"
                        "    return int(b);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(BoolTypeSmokeTest, ConstructorFromInt)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int foo = 42;"
                        "    bool b = bool(foo);\n"
                        "    return int(b);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(BoolTypeSmokeTest, ConstructorFromIntZero)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int foo = 0;"
                        "    bool b = bool(foo);\n"
                        "    return int(b);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(0));
            }

            TEST_F(BoolTypeSmokeTest, OperatorEqual)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    bool b;\n"
                        "    b = true;\n"
                        "    return int(b);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(BoolTypeSmokeTest, OperatorPipePipe)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(false || true);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(BoolTypeSmokeTest, OperatorAmpAmp)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return int(true && false);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(0));
            }
}
