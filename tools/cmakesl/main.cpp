#include "cmake_facade.hpp"
#include "exec/global_executor.hpp"
#include "exec/instance/instance.hpp"

#include <fstream>
#include <iostream>
#include <stack>

class fake_cmake_facade : public cmsl::facade::cmake_facade
{
public:
  version get_cmake_version() const override { return {}; }

  void message(const std::string& msg) const override
  {
    std::cout << msg << '\n';
  }

  void warning(const std::string& msg) const override
  {
    std::cout << msg << '\n';
  }

  void error(const std::string& msg) const override
  {
    std::cout << msg << '\n';
  }

  void fatal_error(const std::string& msg) override
  {
    m_fatal_error_occured = true;
    std::cout << msg << '\n';
  }

  bool did_fatal_error_occure() const override
  {
    return m_fatal_error_occured;
  }

  void register_project(const std::string& name) override {}

  void install(const std::string& target_name,
               const std::string& destination) override
  {
  }

  std::string get_current_binary_dir() const override { return {}; }
  std::string get_current_source_dir() const override { return {}; }
  std::string get_root_source_dir() const override { return {}; }

  void add_custom_command(const std::vector<std::string>& command,
                          const std::string& output) const override
  {
  }

  void add_custom_target(
    const std::string& name,
    const std::vector<std::string>& command) const override
  {
  }

  void make_directory(const std::string& dir) const override {}

  void add_executable(const std::string& name,
                      const std::vector<std::string>& sources) override
  {
  }

  void add_library(const std::string& name,
                   const std::vector<std::string>& sources) override
  {
  }

  void target_link_library(const std::string& target_name,
                           cmsl::facade::visibility v,
                           const std::string& library_name) override
  {
  }

  void target_include_directories(
    const std::string& name, cmsl::facade::visibility v,
    const std::vector<std::string>& sources) override
  {
  }

  void target_compile_definitions(
    const std::string& target_name, cmsl::facade::visibility v,
    const std::vector<std::string>& definitions) override
  {
  }

  std::string current_directory() const override
  {
    return m_directory_stack.top();
  }

  void add_subdirectory_with_old_script(const std::string& dir) override {}

  void prepare_for_add_subdirectory_with_cmakesl_script(
    const std::string& dir) override
  {
  }
  void finalize_after_add_subdirectory_with_cmakesl_script() override {}

  void go_into_subdirectory(const std::string& dir) override
  {
    m_directory_stack.push(dir);
  }

  void go_directory_up() override { m_directory_stack.pop(); }

  void enable_ctest() const override {}

  void add_test(const std::string& test_executable_name) override {}

  system_info get_system_info() const override
  {
    return system_info{ system_id::windows };
  }

  cxx_compiler_info get_cxx_compiler_info() const override
  {
    return cxx_compiler_info{ cxx_compiler_id::clang };
  }

  std::optional<std::string> try_get_extern_define(
    const std::string& name) const override
  {
    return std::nullopt;
  }

  void set_property(const std::string&, const std::string&) const override {}

  std::optional<bool> get_option_value(const std::string& name) const override
  {
    return std::nullopt;
  }

  void register_option(const std::string& name, const std::string& description,
                       bool value) const override
  {
  }

  void set_old_style_variable(const std::string&,
                              const std::string&) const override
  {
  }

  std::string get_old_style_variable(const std::string&) const override
  {
    return {};
  }

  std::string ctest_command() const override { return ""; }

private:
  std::stack<std::string> m_directory_stack;
  std::unique_ptr<cmsl::exec::inst::instance> m_add_subdirectory_result;
  bool m_fatal_error_occured{ false };
};

int main(int argc, const char* argv[])
{
  if (argc < 2) {
    std::cerr << "Usage: cmakesl path/to/root/CMakeLists.cmsl";
    return 1;
  }

  if (argv[1] == std::string{ "--version" }) {
    std::cout << "v0.0";
    return 0;
  }

  const auto root_file_path = std::string{ argv[1] };
  const auto end_of_root_dir = root_file_path.find("/CMakeLists.cmsl");
  if (end_of_root_dir == std::string::npos) {
    std::cerr << "Usage: cmakesl path/to/root/CMakeLists.cmsl";
    return 1;
  }

  const auto root_dir_path = root_file_path.substr(0, end_of_root_dir);

  std::ifstream in{ root_file_path };
  std::string source{ (std::istreambuf_iterator<char>(in)),
                      std::istreambuf_iterator<char>() };

  fake_cmake_facade facade;
  cmsl::exec::global_executor executor{ root_dir_path, facade };

  executor.execute(source);
}
