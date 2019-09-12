#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using CmakeNamespaceSmokeTest = ExecutionSmokeTest;

TEST_F(CmakeNamespaceSmokeTest, ModulePath)
{
  const auto source = "int main()"
                      "{"
                      "    return cmake::module_path.size();"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(0));
}

TEST_F(CmakeNamespaceSmokeTest, CxxCompilerId)
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

TEST_F(CmakeNamespaceSmokeTest, GetCxxCompilerInfo)
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

TEST_F(CmakeNamespaceSmokeTest, CmakeMinimumRequiredIsCallable)
{
  const auto source = "int main()"
                      "{"
                      "    cmake_minimum_required(version(0));"
                      "    return 42;"
                      "}";
  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, Message_CallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::message(\"foo\");"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, message("foo"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, Warning_CallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::warning(\"foo\");"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, warning("foo"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, Error_CallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::error(\"foo\");"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, error("foo"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, FatalError_CallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::fatal_error(\"foo\");"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, fatal_error("foo"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, InstallExecutable_CallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    project p = project(\"p\");"
                      "    list<string> sources;"
                      "    auto exe = p.add_executable(\"exe\", sources);"
                      "    cmake::install(exe);"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, install("exe"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, InstallLibrary_CallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    project p = project(\"p\");"
                      "    list<string> sources;"
                      "    auto lib = p.add_library(\"lib\", sources);"
                      "    cmake::install(lib);"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, install("lib"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, EnableCtest_CallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::enable_ctest();"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, enable_ctest());

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(CmakeNamespaceSmokeTest, AddTest_CallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    project p = project(\"p\");"
                      "    list<string> sources;"
                      "    auto exe = p.add_executable(\"exe\", sources);"
                      "    cmake::add_test(exe);"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, add_test("exe"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
