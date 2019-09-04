#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ImportSmokeTest = ExecutionSmokeTest;

TEST_F(ImportSmokeTest, VariableImport)
{
  const auto source = "import \"import_test/foo.cmsl\""
                      ""
                      "int main()"
                      "{"
                      "    return foo::bar;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ImportSmokeTest, OneVariableInstanceDespiteMultipleImports)
{
  const auto source = "import \"import_test/foo.cmsl\""
                      "import \"import_test/baz.cmsl\""
                      ""
                      "int main()"
                      "{"
                      "    auto result = (foo::bar == 24 && baz::qux == 42);"
                      "    return int(result);"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ImportSmokeTest, FunctionImport)
{
  const auto source = "import \"import_test/foo.cmsl\""
                      ""
                      "int main()"
                      "{"
                      "    return foo::baz();"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ImportSmokeTest, ClassImport)
{
  const auto source = "import \"import_test/foo.cmsl\""
                      ""
                      "int main()"
                      "{"
                      "    foo::qux q;"
                      "    return q.get();"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ImportSmokeTest, EnumImport)
{
  const auto source = "import \"import_test/foo.cmsl\""
                      ""
                      "int main()"
                      "{"
                      "    return int(foo::top::kek.to_string() == \"kek\");"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
