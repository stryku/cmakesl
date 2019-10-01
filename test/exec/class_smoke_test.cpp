#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ClassSmokeTest = ExecutionSmokeTest;

TEST_F(ClassSmokeTest, EmptyClass)
{
  const auto source = "class Foo"
                      "{};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo;"
                      "    return 42;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ClassSmokeTest, MemberAssignment)
{
  const auto source = "class Foo"
                      "{"
                      "    int bar;"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo;"
                      "    foo.bar = 42;"
                      "    return foo.bar;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ClassSmokeTest, ComplexMemberAssignment)
{
  const auto source = "class Foo"
                      "{"
                      "    int bar;"
                      "};"
                      ""
                      "class Baz"
                      "{"
                      "    Foo foo;"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo;"
                      "    foo.bar = 42;"
                      ""
                      "    Baz baz;"
                      "    baz.foo = foo;"
                      "    return baz.foo.bar;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ClassSmokeTest, MemberFunctionReturnTypeDeduction)
{
  const auto source = "class Foo"
                      "{"
                      "    auto bar()"
                      "    {"
                      "        if(false)"
                      "        {"
                      "            return 24;"
                      "        }"
                      "        return 42;"
                      "    }"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo;"
                      "    return foo.bar();"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ClassSmokeTest,
       MemberFunctionReturnTypeDeductionUsingAnotherMethodReturnTypeDeduction)
{
  const auto source = "class Foo"
                      "{"
                      "    auto bar()"
                      "    {"
                      "        return 42;"
                      "    }"
                      ""
                      "    auto baz()"
                      "    {"
                      "        return bar();"
                      "    }"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo;"
                      "    return foo.baz();"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
