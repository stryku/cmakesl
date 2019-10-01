#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ReferenceTypeSmokeTest = ExecutionSmokeTest;

TEST_F(ReferenceTypeSmokeTest, BuiltinType)
{
  const auto source =
    "int foo(int& ref)"
    "{"
    "    ref = 42;"
    "    return 0;"
    "}"
    ""
    "int& set_and_get_ref(int& ref)"
    "{"
    "    ref = 24;"
    "    return ref;"
    "}"
    ""
    "int main()"
    "{"
    "    int i;"
    "    int& i_ref = i;"
    "    bool result = true;"
    "    result = result && i == 0 && i_ref == 0;"
    ""
    "    i = 42;"
    "    result = result && i == 42 && i_ref == 42;"
    ""
    "    i_ref = 24;"
    "    result = result && i == 24 && i_ref == 24;"
    ""
    "    foo(i_ref);"
    "    result = result && i == 42 && i_ref == 42;"
    ""
    "    int& got_ref = set_and_get_ref(i);"
    "    result = result && i == 24 && i_ref == 24 && got_ref == 24;"
    ""
    "    got_ref = 42;"
    "    result = result && i == 42 && i_ref == 42 && got_ref == 42;"
    ""
    "    return int(result);"
    "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ReferenceTypeSmokeTest, UserType)
{
  const auto source =
    "class user_class"
    "{"
    "    int v;"
    "};"
    ""
    "int foo(user_class& ref)"
    "{"
    "    ref.v = 42;"
    "    return 0;"
    "}"
    ""
    "user_class& set_and_get_ref(user_class& ref)"
    "{"
    "    ref.v = 24;"
    "    return ref;"
    "}"
    ""
    "int main()"
    "{"
    "    user_class c;"
    "    user_class& c_ref = c;"
    "    bool result = true;"
    "    result = result && c.v == 0 && c_ref.v == 0;"
    ""
    "    c.v = 42;"
    "    result = result && c.v == 42 && c_ref.v == 42;"
    ""
    "    c_ref.v = 24;"
    "    result = result && c.v == 24 && c_ref.v == 24;"
    ""
    "    foo(c_ref);"
    "    result = result && c.v == 42 && c_ref.v == 42;"
    ""
    "    user_class& got_ref = set_and_get_ref(c);"
    "    result = result && c.v == 24 && c_ref.v == 24 && got_ref.v == 24;"
    ""
    "    got_ref.v = 42;"
    "    result = result && c.v == 42 && c_ref.v == 42 && got_ref.v == 42;"
    ""
    "    return int(result);"
    "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
