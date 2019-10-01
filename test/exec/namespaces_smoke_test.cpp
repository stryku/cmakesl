#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using NamespacesSmokeTest = ExecutionSmokeTest;

TEST_F(NamespacesSmokeTest, VariableInGlobalNamespace)
{
  const auto source = "int foo = 42;"
                      ""
                      "int main()"
                      "{"
                      "    return ::foo;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, VariableInGlobalNamespaceAndLocalScope)
{
  const auto source = "int foo = 42;"
                      ""
                      "int main()"
                      "{"
                      "    int foo = 0;"
                      "    return ::foo;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, VariableInGlobalAndNestedNamespace)
{
  const auto source = "int bar = 0;"
                      ""
                      "namespace foo"
                      "{"
                      "    int bar = 42;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    int foo = 0;"
                      "    return foo::bar;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest,
       VariableInGlobalAndNestedNamespace_GetByGlobalAccess)
{
  const auto source = "int bar = 0;"
                      ""
                      "namespace foo"
                      "{"
                      "    int bar = 42;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    int foo = 0;"
                      "    return ::foo::bar;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, VariableInGlobalAndNestedNamespace_GetFromGlobal)
{
  const auto source = "int bar = 42;"
                      ""
                      "namespace foo"
                      "{"
                      "    int bar = 0;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return ::bar;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, VariableInGlobalAndManyNestedNamespac)
{
  const auto source = "int bar = 0;"
                      ""
                      "namespace foo"
                      "{"
                      "    int bar = 42;"
                      "    namespace baz::qux"
                      "    {"
                      "         int kek = bar;"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return ::foo::baz::qux::kek;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, ClassInGlobalNamespaceAccessByUnqualifiedName)
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
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, ClassInGlobalNamespaceAccessByFullyQualifiedName)
{
  const auto source = "class Foo"
                      "{"
                      "    int bar;"
                      "};"
                      ""
                      "int main()"
                      "{"
                      "    ::Foo foo = { .bar = 42 };"
                      "    return foo.bar;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, ClassInNamespaceAccessByUnqualifiedName)
{
  const auto source = "namespace foo"
                      "{"
                      "    class Bar"
                      "    {"
                      "        int baz;"
                      "    };"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    foo::Bar bar = { .baz = 42 };"
                      "    return bar.baz;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, ClassInSameNamespaceAccessByUnqualifiedName)
{
  const auto source = "namespace foo"
                      "{"
                      "    class Bar"
                      "    {"
                      "        int baz;"
                      "    };"
                      "}"
                      ""
                      "namespace foo"
                      "{"
                      "    Bar bar = { .baz = 42 };"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return foo::bar.baz;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, ClassInNestedNamespaceAccessByUnqualifiedName)
{
  const auto source = "namespace foo"
                      "{"
                      "    class Bar"
                      "    {"
                      "        int baz;"
                      "    };"
                      ""
                      "    namespace qux"
                      "    {"
                      "        class Bar"
                      "        {"
                      "            int kek;"
                      "        };"
                      ""
                      "        Bar top = { .kek = 42 };"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return foo::qux::top.kek;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest,
       ClassInNestedNamespaceAccessClassFromParentNamespace)
{
  const auto source = "namespace foo"
                      "{"
                      "    class Bar"
                      "    {"
                      "        int kek;"
                      "    };"
                      ""
                      "    namespace qux"
                      "    {"
                      "        class Bar"
                      "        {"
                      "            int baz;"
                      "        };"
                      ""
                      "        foo::Bar top = { .kek = 42 };"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return foo::qux::top.kek;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, FunctionInGlobalNamespaceAccessedByUnqualifiedName)
{
  const auto source = "int foo()"
                      "{"
                      "    return 42;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return foo();"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest,
       FunctionInGlobalNamespaceAccessedByFullyQualifiedName)
{
  const auto source = "int foo()"
                      "{"
                      "    return 42;"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return ::foo();"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, FunctionInNamespaceAccessedByQualifiedName)
{
  const auto source = "namespace foo"
                      "{"
                      "    int bar()"
                      "    {"
                      "        return 42;"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return foo::bar();"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, FunctionInNamespaceAccessedByFullyQualifiedName)
{
  const auto source = "namespace foo"
                      "{"
                      "    int bar()"
                      "    {"
                      "        return 42;"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return ::foo::bar();"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, FunctionInSameNamespaceAccessByUnqualifiedName)
{
  const auto source = "namespace foo"
                      "{"
                      "    int bar()"
                      "    {"
                      "        return 42;"
                      "    }"
                      "}"
                      ""
                      "namespace foo"
                      "{"
                      "    int baz = bar();"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return foo::baz;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, FunctionInNestedNamespaceAccessByUnqualifiedName)
{
  const auto source = "namespace foo"
                      "{"
                      "    int bar()"
                      "    {"
                      "        return 0;"
                      "    }"
                      ""
                      "    namespace baz"
                      "    {"
                      "        int bar()"
                      "        {"
                      "            return 42;"
                      "        }"
                      ""
                      "        int qux = bar();"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return foo::baz::qux;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(NamespacesSmokeTest, FunctionInNestedNamespaceAccessByQualifiedName)
{
  const auto source = "namespace foo"
                      "{"
                      "    int bar()"
                      "    {"
                      "        return 42;"
                      "    }"
                      ""
                      "    namespace baz"
                      "    {"
                      "        int bar()"
                      "        {"
                      "            return 0;"
                      "        }"
                      ""
                      "        int qux = foo::bar();"
                      "    }"
                      "}"
                      ""
                      "int main()"
                      "{"
                      "    return foo::baz::qux;"
                      "}";
  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
