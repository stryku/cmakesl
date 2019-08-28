#include "cmake_facade.hpp"
#include "exec/global_executor.hpp"

#include <fstream>
#include <iostream>
#include <stack>

class fake_cmake_facade : public cmsl::facade::cmake_facade
{
public:
  version get_cmake_version() const override { return {}; }

  void message(const std::string&) const override {}
  void warning(const std::string&) const override {}
  void error(const std::string&) const override {}
  void fatal_error(const std::string&) const override {}

  void register_project(const std::string& name) override {}

  std::string get_current_binary_dir() const override { return {}; }

  std::string get_current_source_dir() const override { return {}; }

  void add_executable(const std::string& name,
                      const std::vector<std::string>& sources) override
  {
  }

  void add_library(const std::string& name,
                   const std::vector<std::string>& sources) override
  {
  }

  void target_link_library(const std::string& target_name,
                           const std::string& library_name) override
  {
  }

  std::string current_directory() const override
  {
    return m_directory_stack.top();
  }

  void go_into_subdirectory(const std::string& dir) override
  {
    m_directory_stack.push(dir);
  }

  void go_directory_up() override { m_directory_stack.pop(); }

private:
  std::stack<std::string> m_directory_stack;
};

int main(int argc, const char* argv[])
{
  if (argc < 2) {
    std::cerr << "Usage: cmakesl path/to/directory/with/root/cmakesl/file";
    return 1;
  }

  const auto root_dir_path = std::string{ argv[1] };
  const auto file_path = root_dir_path + "/CMakeLists.cmsl";

  std::ifstream in{ file_path };
  std::string source{ (std::istreambuf_iterator<char>(in)),
                      std::istreambuf_iterator<char>() };

  fake_cmake_facade facade;
  cmsl::exec::global_executor executor{ root_dir_path, facade };

  executor.execute(source);
}
