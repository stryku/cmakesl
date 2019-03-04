#include "exec/source_executor.hpp"
#include "test/exec/mock/cmake_facade_mock.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            using ::testing::Eq;

            class IntTypeSmokeTest : public ::testing::Test
            {
            protected:
                cmake_facade_mock m_facade;
                source_executor m_executor{m_facade};
            };

            TEST_F(IntTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    int i;\n"
                        "    return i;\n"
                        "}";
                const auto result = m_executor.execute2(source);
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
                const auto result = m_executor.execute2(source);
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
                const auto result = m_executor.execute2(source);
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
                const auto result = m_executor.execute2(source);
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
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, OperatorPlus)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return 40 + 2;\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(42));
            }

            TEST_F(IntTypeSmokeTest, OperatorMinus)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    return 100 - 58;\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(42));
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
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(42));
            }
        }
    }
}
