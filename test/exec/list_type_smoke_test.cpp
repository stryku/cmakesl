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

            class ListTypeSmokeTest : public ::testing::Test
            {
            protected:
                cmake_facade_mock m_facade;
                source_executor m_executor{m_facade};
            };

            TEST_F(ListTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    return 1;\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Size)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    return l.size();\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(0));
            }

            TEST_F(ListTypeSmokeTest, OperatorPlusValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    list<int> l2 = l + 42;\n"
                        "    return int(l.size() == 0 && l2.size() == 1);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, OperatorPlusEqualValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l += 42;\n"
                        "    return l.size();\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, OperatorPlusAnotherList)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l += 1;\n"
                        "\n"
                        "    list<int> l2;\n"
                        "    l2 += 2;\n"
                        "    l2 += 3;\n"
                        "\n"
                        "    list<int> l3  = l + l2;\n"
                        "    return int(l3.size() == 3);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, OperatorPlusEqualAnotherList)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l += 42;\n"
                        "    list<int> l2;\n"
                        "    l2 += 42;\n"
                        "    l2 += l;\n"
                        "    return l2.size();\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(2));
            }

            TEST_F(ListTypeSmokeTest, At)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l += 42;\n"
                        "    list<int> l2;\n"
                        "    l2 += 24;\n"
                        "    l2 += l;\n"
                        "    return int(l.at(0) == 42 && l2.at(0) == 24 && l2.at(1) == 42);\n"
                        "}";
                const auto result = m_executor.execute2(source);
                EXPECT_THAT(result, Eq(1));
            }
        }
    }
}
