#include "test/exec/smoke_test_fixture.hpp"

namespace cmsl
{
    namespace exec
    {
        namespace test
        {
            using ::testing::Eq;

            using ListTypeSmokeTest = ExecutionSmokeTest;

            TEST_F(ListTypeSmokeTest, DefaultConstructor)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    return 1;\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, PushBackValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(42);"
                        "    return int(l.size() == 1 && l.at(0) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, PushBackList)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(42);\n"
                        "    list<int> l2;\n"
                        "    l2.push_back(24);\n"
                        "    l2.push_back(l);\n"
                        "    return int(l2.size() == 2 && l2.at(0) == 24 && l2.at(1) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, PushFrontValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(42);\n"
                        "    l.push_front(24);\n"
                        "    return int(l.size() == 2 && l.at(0) == 24 && l.at(1) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, PushFrontList)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(42);\n"
                        "    list<int> l2;\n"
                        "    l2.push_back(24);\n"
                        "    l2.push_front(l);\n"
                        "    return int(l2.size() == 2 && l2.at(0) == 42 && l2.at(1) == 24);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, PopBack)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(24);\n"
                        "    l.pop_back();\n"
                        "    return int(l.size() == 1 && l.at(0) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, PopFront)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(24);\n"
                        "    l.pop_front();\n"
                        "    return int(l.size() == 1 && l.at(0) == 24);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Front)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(24);\n"
                        "    return int(l.size() == 2 && l.front() == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Back)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(24);\n"
                        "    return int(l.size() == 2 && l.back() == 24);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, InsertValue)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.insert(1, 33);\n"
                        "    return int(l.size() == 3 && l.at(1) == 33);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, InsertList)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "\n"
                        "    list<int> l2;\n"
                        "    l2.push_back(33);\n"
                        "    l2.push_back(35);\n"
                        "\n"
                        "    l.insert(1, l2);\n"
                        "    return int(l.size() == 4 && l.at(1) == 33 && l.at(2) == 35);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Erase)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(42);\n"
                        "    l.erase(1);\n"
                        "    return int(l.size() == 2 && l.at(0) == 24 && l.at(1) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, EraseCount)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(35);\n"
                        "    l.push_back(42);\n"
                        "    l.erase(1, 2);\n"
                        "    return int(l.size() == 2 && l.at(0) == 24 && l.at(1) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Remove)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.remove(33);\n"
                        "    return int(l.size() == 2 && l.at(0) == 24 && l.at(1) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, RemoveCount)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.remove(33, 3);\n"
                        "    return int(l.size() == 4\n"
                        "               && l.at(0) == 24\n"
                        "               && l.at(1) == 33\n"
                        "               && l.at(2) == 42\n"
                        "               && l.at(3) == 33);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, RemoveLastCount)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.remove_last(33, 3);\n"
                        "    return int(l.size() == 4\n"
                        "               && l.at(0) == 33\n"
                        "               && l.at(1) == 24\n"
                        "               && l.at(2) == 33\n"
                        "               && l.at(3) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Clear)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(42);\n"
                        "    l.clear();\n"
                        "    return int(l.size() == 0);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Resize)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.resize(4);\n"
                        "    list<int> l2 = l;\n"
                        "    l2.resize(1);\n"
                        "    return int(l.size() == 4\n"
                        "               && l.at(0) == 24\n"
                        "               && l.at(1) == 42\n"
                        "               && l.at(2) == 0\n"
                        "               && l.at(3) == 0\n"
                        "               && l2.size() == 1\n"
                        "               && l2.at(0) == 24);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Sort)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(11);\n"
                        "    l.sort();\n"
                        "    return int(l.size() == 5\n"
                        "               && l.at(0) == 11\n"
                        "               && l.at(1) == 24\n"
                        "               && l.at(2) == 24\n"
                        "               && l.at(3) == 33\n"
                        "               && l.at(4) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Reverse)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(11);\n"
                        "    l.reverse();\n"
                        "    return int(l.size() == 5\n"
                        "               && l.at(0) == 11\n"
                        "               && l.at(1) == 33\n"
                        "               && l.at(2) == 42\n"
                        "               && l.at(3) == 24\n"
                        "               && l.at(4) == 24);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Min)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(55);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(11);\n"
                        "    int min_index = l.min();\n"
                        "    return int(min_index == 4);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Max)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(11);\n"
                        "    int max_index = l.max();\n"
                        "    return int(max_index == 2);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Sublist)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(11);\n"
                        "    list<int> l2 = l.sublist(1);\n"
                        "    return int(l2.size() == 4"
                        "               && l2.at(0) == 24\n"
                        "               && l2.at(1) == 42\n"
                        "               && l2.at(2) == 33\n"
                        "               && l2.at(3) == 11);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, SublistCount)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(11);\n"
                        "    list<int> l2 = l.sublist(1, 2);\n"
                        "    return int(l2.size() == 2"
                        "               && l2.at(0) == 24\n"
                        "               && l2.at(1) == 42);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Empty)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    list<int> l2;\n"
                        "    return int(l.empty() == false && l2.empty());\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, Find)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(11);\n"
                        "    int found_at = l.find(42);\n"
                        "    int found_failed = l.find(999);\n"
                        "    return int(found_at == 2 && found_failed == int(0 - 1));\n" // Todo: Change (0 - 1) to -1 when unary operators are implemented
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, FindPos)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(42);\n"
                        "    l.push_back(33);\n"
                        "    l.push_back(24);\n"
                        "    l.push_back(11);\n"
                        "    int found_at = l.find(24, 2);\n"
                        "    return int(found_at == 4);\n"
                        "}";
                const auto result = m_executor.execute(source);
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
                const auto result = m_executor.execute(source);
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
                const auto result = m_executor.execute(source);
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
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, OperatorPlusList)
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
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, OperatorPlusEqualList)
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
                const auto result = m_executor.execute(source);
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
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }

            TEST_F(ListTypeSmokeTest, InitializerList)
            {
                const auto source =
                        "int main()\n"
                        "{\n"
                        "    list<int> l;\n"
                        "    l += 1;\n"
                        "    l += { 2, 6 };\n"
                        "    l.insert(2, { 3, 4, 5 });\n"
                        "    return int(l.size() == 6\n"
                        "               && l.at(0) == 1\n"
                        "               && l.at(1) == 2\n"
                        "               && l.at(2) == 3\n"
                        "               && l.at(3) == 4\n"
                        "               && l.at(4) == 5\n"
                        "               && l.at(5) == 6);\n"
                        "}";
                const auto result = m_executor.execute(source);
                EXPECT_THAT(result, Eq(1));
            }
        }
    }
}
