#include "exec/instance/project_value.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/list_value_utils.hpp"

#include "cmake_facade.hpp"

namespace cmsl::exec::inst {
project_value::project_value(const std::string& name)
  : m_name{ name }
{
}

std::string project_value::name() const
{
  return m_name;
}

void project_value::add_executable(facade::cmake_facade& cmake_facade,
                                   const std::string& name,
                                   const list_value& sources)
{
  const auto prefix = cmake_facade.current_directory() + '/';
  const auto collected_sources = list_value_utils{ sources }.strings(prefix);
  cmake_facade.add_executable(name, collected_sources);
}

void project_value::add_library(facade::cmake_facade& cmake_facade,
                                const std::string& name,
                                const list_value& sources)
{
  const auto prefix = cmake_facade.current_directory() + '/';
  const auto collected_sources = list_value_utils{ sources }.strings(prefix);
  cmake_facade.add_library(name, collected_sources);
}
}
