#pragma once

#include "cmake_facade.hpp"

#include "exec/instance/instance.hpp"

#include <gmock/gmock.h>

class cmMakefile
{
};

namespace cmsl::exec::test {
class cmake_facade_mock : public facade::cmake_facade
{
public:
  MOCK_CONST_METHOD0(get_cmake_version, version());
  MOCK_CONST_METHOD1(message, void(const std::string&));
  MOCK_CONST_METHOD1(warning, void(const std::string&));
  MOCK_CONST_METHOD1(error, void(const std::string&));
  MOCK_METHOD1(fatal_error, void(const std::string&));
  MOCK_CONST_METHOD0(did_fatal_error_occure, bool());

  MOCK_METHOD1(register_project, void(const std::string&));
  MOCK_METHOD2(install, void(const std::string&, const std::string&));

  MOCK_CONST_METHOD0(get_current_binary_dir, std::string());
  MOCK_CONST_METHOD0(get_current_source_dir, std::string());
  MOCK_CONST_METHOD0(get_root_source_dir, std::string());

  MOCK_CONST_METHOD2(add_custom_command,
                     void(const std::vector<std::string>&,
                          const std::string&));

  MOCK_CONST_METHOD2(add_custom_target,
                     void(const std::string&,
                          const std::vector<std::string>&));

  MOCK_CONST_METHOD1(make_directory, void(const std::string&));

  MOCK_METHOD2(add_executable,
               void(const std::string&, const std::vector<std::string>&));
  MOCK_METHOD2(add_library,
               void(const std::string&, const std::vector<std::string>&));
  MOCK_METHOD3(target_link_library,
               void(const std::string&, facade::visibility,
                    const std::string&));
  MOCK_METHOD3(target_include_directories,
               void(const std::string&, facade::visibility,
                    const std::vector<std::string>&));
  MOCK_METHOD3(target_compile_definitions,
               void(const std::string&, facade::visibility v,
                    const std::vector<std::string>&));

  MOCK_CONST_METHOD0(current_directory, std::string());
  MOCK_METHOD0(setup_adding_subdirectory_wiith_cmakesl_script, void());
  MOCK_METHOD1(go_into_subdirectory, void(const std::string&));
  MOCK_METHOD1(go_into_subdirectory, void(cmMakefile&));
  MOCK_METHOD1(add_subdirectory_with_cmakesl_script, void(const std::string&));
  MOCK_METHOD1(set_add_subdirectory_with_cmakesl_script_result,
               void(std::unique_ptr<exec::inst::instance>));
  MOCK_METHOD0(gather_add_subdirectory_with_cmakesl_script_result,
               std::unique_ptr<exec::inst::instance>());
  MOCK_METHOD1(add_subdirectory_with_old_script, void(const std::string&));
  MOCK_METHOD0(go_directory_up, void());

  MOCK_METHOD1(prepare_for_add_subdirectory_with_cmakesl_script,
               void(const std::string&));
  MOCK_METHOD0(finalize_after_add_subdirectory_with_cmakesl_script, void());

  MOCK_CONST_METHOD0(enable_ctest, void());
  MOCK_METHOD1(add_test, void(const std::string&));

  MOCK_CONST_METHOD0(get_system_info, system_info());
  MOCK_CONST_METHOD0(get_cxx_compiler_info, cxx_compiler_info());
  MOCK_CONST_METHOD1(try_get_extern_define,
                     std::optional<std::string>(const std::string&));

  MOCK_CONST_METHOD2(set_property,
                     void(const std::string&, const std::string&));

  MOCK_CONST_METHOD1(get_option_value,
                     std::optional<bool>(const std::string&));

  MOCK_CONST_METHOD3(register_option,
                     void(const std::string&, const std::string&, bool));

  MOCK_CONST_METHOD2(set_old_style_variable,
                     void(const std::string&, const std::string&));

  MOCK_CONST_METHOD1(get_old_style_variable, std::string(const std::string&));

  MOCK_CONST_METHOD0(ctest_command, std::string());
};
}
