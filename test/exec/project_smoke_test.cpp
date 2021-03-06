#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using ::testing::Return;

using ProjectSmokeTest = ExecutionSmokeTest;

TEST_F(ProjectSmokeTest, ConstructorCallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::project p = cmake::project(\"foo\");"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, register_project("foo"));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ProjectSmokeTest, AddExecutableCallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::project p = cmake::project(\"foo\");"
                      "    list<string> sources = { \"main.cpp\" };"
                      "    p.add_executable(\"exe\", sources);"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, current_directory()).WillRepeatedly(Return("foo"));

  std::vector<std::string> expected_sources = { "main.cpp" };
  EXPECT_CALL(m_facade, add_executable("exe", expected_sources));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ProjectSmokeTest, AddLibraryCallsFacadeMethod)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::project p = cmake::project(\"foo\");"
                      "    list<string> sources = { \"lib.cpp\" };"
                      "    p.add_library(\"lib\", sources);"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, current_directory()).WillRepeatedly(Return("foo"));

  std::vector<std::string> expected_sources = { "lib.cpp" };
  EXPECT_CALL(m_facade, add_library("lib", expected_sources));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(ProjectSmokeTest, FindLibrary)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::project p = cmake::project(\"foo\");"
                      "    p.add_library(\"lib\", {\"foo\"});"
                      "    auto l = p.find_library(\"lib\");"
                      "    return int(l.name() == \"lib\");"
                      "}";

  EXPECT_CALL(m_facade, current_directory()).WillRepeatedly(Return("foo"));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(1));
}
}
