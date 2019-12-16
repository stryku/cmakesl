#include "test/exec/mock/instance_mock.hpp"
#include "test/exec/smoke_test_fixture.hpp"

#include <gmock/gmock.h>

namespace cmsl::exec::test {
using ::testing::Eq;
using ::testing::Return;
using ::testing::_;

using DeclarativeFormatSmokeTest = ExecutionSmokeTest;

TEST_F(DeclarativeFormatSmokeTest, AddDeclarativeFileWithStaticLibrary)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::add_declarative_file(\"static_library.dcmsl\");"
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "library_name";

  const std::vector<std::string> expected_sources = { "library.cpp" };
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

  const std::vector<std::string> expected_include_dirs = { "include" };
  EXPECT_CALL(m_facade,
              target_include_directories(_, _, expected_include_dirs));

  const std::vector<std::string> expected_compile_options = { "-DANSWER=42" };
  EXPECT_CALL(m_facade,
              target_compile_definitions(_, _, expected_compile_options));

  const std::string expected_dependency = "dependency";
  const std::string expected_another_dependency = "another_dependency";
  EXPECT_CALL(m_facade,
              target_link_library(library_name, _, expected_dependency));
  EXPECT_CALL(
    m_facade,
    target_link_library(library_name, _, expected_another_dependency));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DeclarativeFormatSmokeTest, AddDeclarativeFileWithExecutable)
{
  const auto source = "int main()"
                      "{"
                      "    cmake::add_declarative_file(\"executable.dcmsl\");"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto executable_name = "executable_name";

  const std::vector<std::string> expected_sources = { "exec.cpp" };
  EXPECT_CALL(m_facade, add_executable(executable_name, expected_sources));

  const std::vector<std::string> expected_include_dirs = { "include" };
  EXPECT_CALL(m_facade,
              target_include_directories(_, _, expected_include_dirs));

  const std::vector<std::string> expected_compile_options = { "-DANSWER=42" };
  EXPECT_CALL(m_facade,
              target_compile_definitions(_, _, expected_compile_options));

  const std::string expected_dependency = "dependency";
  const std::string expected_another_dependency = "another_dependency";
  EXPECT_CALL(m_facade,
              target_link_library(executable_name, _, expected_dependency));
  EXPECT_CALL(
    m_facade,
    target_link_library(executable_name, _, expected_another_dependency));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
