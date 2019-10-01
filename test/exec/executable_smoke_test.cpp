#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using ExecutableSmokeTest = ExecutionSmokeTest;

TEST_F(ExecutableSmokeTest, NameReturnsProperName)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::project p = cmake::project(\"foo\");"
    "    list<string> sources;"
    "    cmake::executable exe = p.add_executable(\"some name\", sources);"
    ""
    "    string name = exe.name();"
    "    return int(name == \"some name\");"
    "}";

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(ExecutableSmokeTest, LinkToCallsFacadeMethod)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::project p = cmake::project(\"foo\");"
    "    list<string> sources;"
    ""
    "    cmake::library some_lib = p.add_library(\"some name\", sources);"
    ""
    "    cmake::executable exe = p.add_executable(\"exe\", sources);"
    "    exe.link_to(some_lib);"
    ""
    "    return 42;"
    "}";

  EXPECT_CALL(
    m_facade,
    target_link_library("exe", facade::visibility::private_, "some name"));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ExecutableSmokeTest, LinkToWithVisibilityCallsFacadeMethod)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::project p = cmake::project(\"foo\");"
    "    list<string> sources;"
    ""
    "    cmake::library some_lib = p.add_library(\"some name\", sources);"
    ""
    "    cmake::executable exe = p.add_executable(\"exe\", sources);"
    "    exe.link_to(some_lib, cmake::visibility::interface);"
    ""
    "    return 42;"
    "}";

  EXPECT_CALL(
    m_facade,
    target_link_library("exe", facade::visibility::interface, "some name"));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ExecutableSmokeTest, IncludeDirectoriesCallsFacadeMethod)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::project p = cmake::project(\"foo\");"
    "    list<string> sources;"
    "    cmake::library lib = p.add_library(\"exe\", sources);"
    ""
    "    list<string> include_dirs = { \"dir\" };"
    "    lib.include_directories(include_dirs);"
    "    return 42;"
    "}";

  std::vector<std::string> expected_dirs = { "dir" };
  EXPECT_CALL(m_facade,
              target_include_directories("exe", facade::visibility::private_,
                                         expected_dirs));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ExecutableSmokeTest, IncludeDirectoriesWithVisibilityCallsFacadeMethod)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::project p = cmake::project(\"foo\");"
    "    list<string> sources;"
    "    cmake::library lib = p.add_library(\"exe\", sources);"
    ""
    "    list<string> include_dirs = { \"dir\" };"
    "    lib.include_directories(include_dirs, cmake::visibility::interface);"
    "    return 42;"
    "}";

  std::vector<std::string> expected_dirs = { "dir" };
  EXPECT_CALL(m_facade,
              target_include_directories("exe", facade::visibility::interface,
                                         expected_dirs));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
