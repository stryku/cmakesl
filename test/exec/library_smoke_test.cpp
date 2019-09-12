#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;

using LibrarySmokeTest = ExecutionSmokeTest;

TEST_F(LibrarySmokeTest, NameReturnsProperName)
{
  const auto source =
    "int main()"
    "{"
    "    project p = project(\"foo\");"
    "    list<string> sources;"
    "    library lib = p.add_library(\"some name\", sources);"
    ""
    "    string name = lib.name();"
    "    return int(name == \"some name\");"
    "}";

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(1));
}

TEST_F(LibrarySmokeTest, LinkToCallsFacadeMethod)
{
  const auto source =
    "int main()"
    "{"
    "    project p = project(\"foo\");"
    "    list<string> sources;"
    ""
    "    library some_lib = p.add_library(\"some name\", sources);"
    ""
    "    library lib = p.add_library(\"lib\", sources);"
    "    lib.link_to(some_lib);"
    ""
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, target_link_library("lib", "some name"));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(LibrarySmokeTest, IncludeDirectoriesCallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    project p = project(\"foo\");"
                      "    list<string> sources;"
                      "    library lib = p.add_library(\"lib\", sources);"
                      ""
                      "    list<string> include_dirs = { \"dir\" };"
                      "    lib.include_directories(include_dirs);"
                      "    return 42;"
                      "}";

  std::vector<std::string> expected_dirs = { "dir" };
  EXPECT_CALL(m_facade, target_include_directories("lib", expected_dirs));

  const auto result = m_executor.execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
