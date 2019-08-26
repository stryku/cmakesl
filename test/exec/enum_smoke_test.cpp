#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using EnumSmokeTest = ExecutionSmokeTest;

TEST_F(EnumSmokeTest, EmptyEnum)
{
  const auto source = "enum Foo {};"
                      ""
                      "int main()"
                      "{"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(EnumSmokeTest, EnumWithEnumerators)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(EnumSmokeTest, EnumCreation)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo;"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(EnumSmokeTest, EnumAssignment)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(EnumSmokeTest, EnumReassignment)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    foo = Foo::baz;"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(EnumSmokeTest, AssignmentFromAnotherVariable)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    Foo foo2 = Foo::qux;"
                      "    foo = foo2;"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(EnumSmokeTest, EqualToConstant)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    return int(foo == Foo::bar);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(EnumSmokeTest, EqualToAnotherVariable)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    Foo foo2 = Foo::bar;"
                      "    return int(foo == foo2);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(EnumSmokeTest, FailedEqualWithConstant)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    return int(foo == Foo::qux);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(EnumSmokeTest, FailedEqualWithAnotherVariable)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    Foo foo2 = Foo::baz;"
                      "    return int(foo == foo2);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(EnumSmokeTest, NotEqualToConstant)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    return int(foo != Foo::qux);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(EnumSmokeTest, NotEqualToAnotherVariable)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    Foo foo2 = Foo::baz;"
                      "    return int(foo != foo2);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(EnumSmokeTest, FailedNotEqualWithConstant)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    return int(foo != Foo::bar);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(EnumSmokeTest, FailedNotEqualWithAnotherVariable)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = Foo::bar;"
                      "    Foo foo2 = Foo::bar;"
                      "    return int(foo != foo2);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(EnumSmokeTest, ConstantToString)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    auto str = Foo::baz.to_string();"
                      "    return int(str == \"baz\");"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(EnumSmokeTest, ValueToString)
{
  const auto source = "enum Foo "
                      "{"
                      "    bar,"
                      "    baz,"
                      "    qux"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    auto top = Foo::qux;"
                      "    auto str = top.to_string();"
                      "    return int(str == \"qux\");"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
