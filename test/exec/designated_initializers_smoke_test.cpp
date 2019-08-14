#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using DesignatedInitializersSmokeTest = ExecutionSmokeTest;

TEST_F(DesignatedInitializersSmokeTest, MemberInitialization)
{
  const auto source = "class Foo"
                      "{"
                      "    int bar;"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    Foo foo = { .bar = 42 };"
                      "    return foo.bar;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DesignatedInitializersSmokeTest, NestedMemberInitialization)
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
                      "    Baz baz = { .foo = { .bar = 42 } };"
                      "    return baz.foo.bar;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DesignatedInitializersSmokeTest, FunctionCallParameter)
{
  const auto source = "class Foo"
                      "{"
                      "    int bar;"
                      "};"
                      ""
                      "auto baz(Foo foo)"
                      "{"
                      "    return foo.bar;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    auto qux = baz( { .bar = 42 } );"
                      "    return qux;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
