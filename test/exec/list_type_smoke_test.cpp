#include "test/exec/smoke_test_fixture.hpp"

namespace cmsl::exec::test {
using ::testing::Eq;

using ListTypeSmokeTest = ExecutionSmokeTest;

TEST_F(ListTypeSmokeTest, DefaultConstructor)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    return 1;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, PushBackValue)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(42);"
                      "    return int(l.size() == 1 && l.at(0) == 42);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, PushBackList)
{
  const auto source =
    "int main()"
    "{"
    "    list<int> l;"
    "    l.push_back(42);"
    "    list<int> l2;"
    "    l2.push_back(24);"
    "    l2.push_back(l);"
    "    return int(l2.size() == 2 && l2.at(0) == 24 && l2.at(1) == 42);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, PushFrontValue)
{
  const auto source =
    "int main()"
    "{"
    "    list<int> l;"
    "    l.push_back(42);"
    "    l.push_front(24);"
    "    return int(l.size() == 2 && l.at(0) == 24 && l.at(1) == 42);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, PushFrontList)
{
  const auto source =
    "int main()"
    "{"
    "    list<int> l;"
    "    l.push_back(42);"
    "    list<int> l2;"
    "    l2.push_back(24);"
    "    l2.push_front(l);"
    "    return int(l2.size() == 2 && l2.at(0) == 42 && l2.at(1) == 24);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, PopBack)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(42);"
                      "    l.push_back(24);"
                      "    l.pop_back();"
                      "    return int(l.size() == 1 && l.at(0) == 42);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, PopFront)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(42);"
                      "    l.push_back(24);"
                      "    l.pop_front();"
                      "    return int(l.size() == 1 && l.at(0) == 24);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Front)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(42);"
                      "    l.push_back(24);"
                      "    return int(l.size() == 2 && l.front() == 42);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Back)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(42);"
                      "    l.push_back(24);"
                      "    return int(l.size() == 2 && l.back() == 24);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, InsertValue)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.insert(1, 33);"
                      "    return int(l.size() == 3 && l.at(1) == 33);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, InsertList)
{
  const auto source =
    "int main()"
    "{"
    "    list<int> l;"
    "    l.push_back(24);"
    "    l.push_back(42);"
    ""
    "    list<int> l2;"
    "    l2.push_back(33);"
    "    l2.push_back(35);"
    ""
    "    l.insert(1, l2);"
    "    return int(l.size() == 4 && l.at(1) == 33 && l.at(2) == 35);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Erase)
{
  const auto source =
    "int main()"
    "{"
    "    list<int> l;"
    "    l.push_back(24);"
    "    l.push_back(33);"
    "    l.push_back(42);"
    "    l.erase(1);"
    "    return int(l.size() == 2 && l.at(0) == 24 && l.at(1) == 42);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, EraseCount)
{
  const auto source =
    "int main()"
    "{"
    "    list<int> l;"
    "    l.push_back(24);"
    "    l.push_back(33);"
    "    l.push_back(35);"
    "    l.push_back(42);"
    "    l.erase(1, 2);"
    "    return int(l.size() == 2 && l.at(0) == 24 && l.at(1) == 42);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Remove)
{
  const auto source =
    "int main()"
    "{"
    "    list<int> l;"
    "    l.push_back(33);"
    "    l.push_back(24);"
    "    l.push_back(33);"
    "    l.push_back(33);"
    "    l.push_back(33);"
    "    l.push_back(42);"
    "    l.push_back(33);"
    "    l.remove(33);"
    "    return int(l.size() == 2 && l.at(0) == 24 && l.at(1) == 42);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, RemoveCount)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(33);"
                      "    l.push_back(24);"
                      "    l.push_back(33);"
                      "    l.push_back(33);"
                      "    l.push_back(33);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.remove(33, 3);"
                      "    return int(l.size() == 4"
                      "               && l.at(0) == 24"
                      "               && l.at(1) == 33"
                      "               && l.at(2) == 42"
                      "               && l.at(3) == 33);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, RemoveLastCount)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(33);"
                      "    l.push_back(24);"
                      "    l.push_back(33);"
                      "    l.push_back(33);"
                      "    l.push_back(33);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.remove_last(33, 3);"
                      "    return int(l.size() == 4"
                      "               && l.at(0) == 33"
                      "               && l.at(1) == 24"
                      "               && l.at(2) == 33"
                      "               && l.at(3) == 42);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Clear)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(33);"
                      "    l.push_back(42);"
                      "    l.clear();"
                      "    return int(l.size() == 0);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Resize)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.resize(4);"
                      "    list<int> l2 = l;"
                      "    l2.resize(1);"
                      "    return int(l.size() == 4"
                      "               && l.at(0) == 24"
                      "               && l.at(1) == 42"
                      "               && l.at(2) == 0"
                      "               && l.at(3) == 0"
                      "               && l2.size() == 1"
                      "               && l2.at(0) == 24);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Sort)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.push_back(11);"
                      "    l.sort();"
                      "    return int(l.size() == 5"
                      "               && l.at(0) == 11"
                      "               && l.at(1) == 24"
                      "               && l.at(2) == 24"
                      "               && l.at(3) == 33"
                      "               && l.at(4) == 42);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Reverse)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.push_back(11);"
                      "    l.reverse();"
                      "    return int(l.size() == 5"
                      "               && l.at(0) == 11"
                      "               && l.at(1) == 33"
                      "               && l.at(2) == 42"
                      "               && l.at(3) == 24"
                      "               && l.at(4) == 24);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Min)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(24);"
                      "    l.push_back(55);"
                      "    l.push_back(42);"
                      "    l.push_back(11);"
                      "    int min_index = l.min();"
                      "    return int(min_index == 4);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Max)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.push_back(11);"
                      "    int max_index = l.max();"
                      "    return int(max_index == 2);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Sublist)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.push_back(11);"
                      "    list<int> l2 = l.sublist(1);"
                      "    return int(l2.size() == 4"
                      "               && l2.at(0) == 24"
                      "               && l2.at(1) == 42"
                      "               && l2.at(2) == 33"
                      "               && l2.at(3) == 11);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, SublistCount)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.push_back(11);"
                      "    list<int> l2 = l.sublist(1, 2);"
                      "    return int(l2.size() == 2"
                      "               && l2.at(0) == 24"
                      "               && l2.at(1) == 42);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Empty)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    list<int> l2;"
                      "    return int(l.empty() == false && l2.empty());"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Find)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.push_back(11);"
                      "    int found_at = l.find(42);"
                      "    int found_failed = l.find(999);"
                      "    return int(found_at == 2 && found_failed == int(0 "
                      "- 1));" // Todo: Change (0 - 1) to -1 when unary
                               // operators are implemented
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, FindPos)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l.push_back(24);"
                      "    l.push_back(24);"
                      "    l.push_back(42);"
                      "    l.push_back(33);"
                      "    l.push_back(24);"
                      "    l.push_back(11);"
                      "    int found_at = l.find(24, 2);"
                      "    return int(found_at == 4);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, Size)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    return l.size();"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(ListTypeSmokeTest, OperatorPlusValue)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    list<int> l2 = l + 42;"
                      "    return int(l.size() == 0 && l2.size() == 1);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, OperatorPlusEqualValue)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l += 42;"
                      "    return l.size();"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, OperatorPlusList)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l += 1;"
                      ""
                      "    list<int> l2;"
                      "    l2 += 2;"
                      "    l2 += 3;"
                      ""
                      "    list<int> l3  = l + l2;"
                      "    return int(l3.size() == 3);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, OperatorPlusEqualList)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l += 42;"
                      "    list<int> l2;"
                      "    l2 += 42;"
                      "    l2 += l;"
                      "    return l2.size();"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(2));
}

TEST_F(ListTypeSmokeTest, At)
{
  const auto source =
    "int main()"
    "{"
    "    list<int> l;"
    "    l += 42;"
    "    list<int> l2;"
    "    l2 += 24;"
    "    l2 += l;"
    "    return int(l.at(0) == 42 && l2.at(0) == 24 && l2.at(1) == 42);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ListTypeSmokeTest, InitializerList)
{
  const auto source = "int main()"
                      "{"
                      "    list<int> l;"
                      "    l += 1;"
                      "    l += { 2, 6 };"
                      "    l.insert(2, { 3, 4, 5 });"
                      "    return int(l.size() == 6"
                      "               && l.at(0) == 1"
                      "               && l.at(1) == 2"
                      "               && l.at(2) == 3"
                      "               && l.at(3) == 4"
                      "               && l.at(4) == 5"
                      "               && l.at(5) == 6);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
