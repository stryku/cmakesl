#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using NamespacesSmokeTest = ExecutionSmokeTest;

TEST_F(NamespacesSmokeTest, VariableInGlobalNamespace)
{
  const auto source = "int foo = 42;\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    return ::foo;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, VariableInGlobalNamespaceAndLocalScope)
{
  const auto source = "int foo = 42;\n"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    int foo = 0;"
                      "    return ::foo;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, VariableInGlobalAndNestedNamespace)
{
  const auto source = "int bar = 0;\n"
                      ""
                      "namespace foo"
                      "{"
                      "    int bar = 42;"
                      "}"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    int foo = 0;"
                      "    return foo::bar;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest,
       VariableInGlobalAndNestedNamespace_GetByGlobalAccess)
{
  const auto source = "int bar = 0;\n"
                      ""
                      "namespace foo"
                      "{"
                      "    int bar = 42;"
                      "}"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    int foo = 0;"
                      "    return ::foo::bar;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, VariableInGlobalAndNestedNamespace_GetFromGlobal)
{
  const auto source = "int bar = 42;\n"
                      ""
                      "namespace foo"
                      "{"
                      "    int bar = 0;"
                      "}"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    return ::bar;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, VariableInGlobalAndManyNestedNamespac)
{
  const auto source = "int bar = 0;\n"
                      ""
                      "namespace foo"
                      "{"
                      "    int bar = 42;"
                      "    namespace baz::qux"
                      "    {"
                      "         int kek = bar;"
                      "    }"
                      "}"
                      "\n"
                      "int main()\n"
                      "{\n"
                      "    return ::foo::baz::qux::kek;\n"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

}
