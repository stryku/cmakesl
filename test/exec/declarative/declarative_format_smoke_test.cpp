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
              target_compile_options(_, _, expected_compile_options));

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

TEST_F(DeclarativeFormatSmokeTest,
       AddDeclarativeFileWithStaticLibraryWithNameSuffix)
{
  const auto source =
    "int main()"
    "{"
    "    "
    "cmake::add_declarative_file(\"static_library_with_name_suffix.dcmsl\");"
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "library_name_suffix";

  const std::vector<std::string> expected_sources = { "library.cpp" };
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DeclarativeFormatSmokeTest,
       AddDeclarativeFileWithStaticLibraryWithNamePrefix)
{
  const auto source =
    "int main()"
    "{"
    "    "
    "cmake::add_declarative_file(\"static_library_with_name_prefix.dcmsl\");"
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "prefix_library_name";

  const std::vector<std::string> expected_sources = { "library.cpp" };
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

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
              target_compile_options(_, _, expected_compile_options));

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

TEST_F(DeclarativeFormatSmokeTest, AddDeclarativeFileWithTestExecutable)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::add_declarative_file(\"test_executable.dcmsl\");"
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto executable_name = "test_executable_name";

  const std::vector<std::string> expected_sources = { "test_exec.cpp" };
  EXPECT_CALL(m_facade, add_executable(executable_name, expected_sources));
  EXPECT_CALL(m_facade, add_test(executable_name));

  const std::vector<std::string> expected_include_dirs = { "include" };
  EXPECT_CALL(m_facade,
              target_include_directories(_, _, expected_include_dirs));

  const std::vector<std::string> expected_compile_options = { "-DANSWER=42" };
  EXPECT_CALL(m_facade,
              target_compile_options(_, _, expected_compile_options));

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

TEST_F(DeclarativeFormatSmokeTest,
       AddSubdirectoryWithDeclarativeFileWithStaticLibrary)
{
  const auto source = "int main()"
                      "{"
                      "    add_subdirectory(\"foo\");"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "foo";

  const std::vector<std::string> expected_sources = { "foo.cpp" };
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DeclarativeFormatSmokeTest,
       AddDeclarativeFileWithStaticLibraryUsingCmakeVariablesAccessor)
{
  const auto source = "int main()"
                      "{"
                      "    "
                      "cmake::add_declarative_file(\"static_library_with_"
                      "cmake_variables.dcmsl\");"
                      "    return 42;"
                      "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "foo";
  constexpr auto library_public_files = "file1.cpp;file2.cpp";

  EXPECT_CALL(m_facade, get_old_style_variable("LIBRARY_NAME"))
    .WillOnce(Return(library_name));
  EXPECT_CALL(m_facade, get_old_style_variable("LIBRARY_PUBLIC_FILES"))
    .WillOnce(Return(library_public_files));

  const std::vector<std::string> expected_sources = { "file1.cpp",
                                                      "file2.cpp" };
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DeclarativeFormatSmokeTest, AddDeclarativeFileWithCustomStaticLibrary)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::add_declarative_file(\"custom_static_library.dcmsl\");"
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "library_name";

  const std::vector<std::string> expected_sources = {};
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

  const std::string expected_dependency = "custom_dependency";
  EXPECT_CALL(m_facade,
              target_link_library(library_name, _, expected_dependency));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DeclarativeFormatSmokeTest,
       AddDeclarativeFileWithImportedCustomStaticLibrary)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::add_declarative_file(\"import_custom_static_library.dcmsl\");"
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "library_name";

  const std::vector<std::string> expected_sources = {};
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

  const std::string expected_dependency = "custom_dependency";
  EXPECT_CALL(m_facade,
              target_link_library(library_name, _, expected_dependency));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DeclarativeFormatSmokeTest,
       AddDeclarativeFileWithCustomStaticLibraryWithListAppending)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::add_declarative_file(\"custom_static_library_"
    "with_list_appending.dcmsl\");"
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "library_name";

  const std::vector<std::string> expected_sources = {};
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

  const std::string expected_dependency = "custom_dependency";
  const std::string expected_another_dependency = "another_custom_dependency";
  EXPECT_CALL(m_facade,
              target_link_library(library_name, _, expected_dependency));
  EXPECT_CALL(
    m_facade,
    target_link_library(library_name, _, expected_another_dependency));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}

TEST_F(DeclarativeFormatSmokeTest,
       AddDeclarativeFileWithCustomStaticLibraryWithStringAppending)
{
  const auto source =
    "int main()"
    "{"
    "    cmake::add_declarative_file(\"custom_static_library_"
    "with_string_appending.dcmsl\");"
    "    return 42;"
    "}";

  EXPECT_CALL(m_facade, current_directory())
    .WillRepeatedly(Return(CMAKESL_EXEC_SMOKE_TEST_ROOT_DIR +
                           std::string{ "/declarative" }));

  constexpr auto library_name = "library_name";

  const std::vector<std::string> expected_sources = {};
  EXPECT_CALL(m_facade, add_library(library_name, expected_sources));

  const std::string expected_dependency = "custom_dependency";
  const std::string expected_another_dependency = "another_custom_dependency";
  EXPECT_CALL(m_facade,
              target_link_library(library_name, _, expected_dependency));
  EXPECT_CALL(
    m_facade,
    target_link_library(library_name, _, expected_another_dependency));

  const auto result = m_executor->execute(source);
  EXPECT_THAT(result, Eq(42));
}
}
