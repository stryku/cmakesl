#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using BuiltinCMakeNamespaceSmokeTest = ExecutionSmokeTest;

TEST_F(BuiltinCMakeNamespaceSmokeTest, ModulePath)
{
  const auto source = "int main()"
                      "{"
                      "    return cmake::module_path.size();"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(BuiltinCMakeNamespaceSmokeTest, CxxCompilerId)
{
  const auto source =
    "int main()"
    "{"
    "    auto result = cmake::cxx_compiler_id::clang.to_string() == \"clang\";"
    "    return int(result);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(BuiltinCMakeNamespaceSmokeTest, GetCxxCompilerInfo)
{
  const auto source =
    "int main()"
    "{"
    "    auto info = cmake::get_cxx_compiler_info();"
    "    auto is_clang = info.id == cmake::cxx_compiler_id::clang;"
    "    return int(is_clang);"
    "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

}
