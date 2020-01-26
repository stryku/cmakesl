#include "exec/instance/target_value.hpp"
#include "exec/instance/list_value_utils.hpp"

#include "cmake_facade.hpp"

namespace cmsl::exec::inst {
target_value::target_value(std::string name)
  : m_name{ std::move(name) }
{
}

std::string target_value::name() const
{
  return m_name;
}

void target_value::link_to(facade::cmake_facade& cmake_facade,
                           facade::visibility v,
                           const target_value& target) const
{
  link_to(cmake_facade, v, target.name());
}

void target_value::include_directories(facade::cmake_facade& cmake_facade,
                                       facade::visibility v,
                                       const list_value& dirs) const
{
  const auto dir_values = list_value_utils{ dirs }.strings();
  if (dir_values.empty()) {
    return;
  }

  cmake_facade.target_include_directories(m_name, v, dir_values);
}

void target_value::compile_definitions(facade::cmake_facade& cmake_facade,
                                       const list_value& definitions_list,
                                       facade::visibility v) const
{
  const auto definitions = list_value_utils{ definitions_list }.strings();
  if (definitions.empty()) {
    return;
  }
  cmake_facade.target_compile_definitions(m_name, v, definitions);
}

void target_value::compile_options(facade::cmake_facade& cmake_facade,
                                   const list_value& options_list,
                                   facade::visibility v) const
{
  const auto options = list_value_utils{ options_list }.strings();
  if (options.empty()) {
    return;
  }
  cmake_facade.target_compile_options(m_name, v, options);
}

void target_value::link_to(facade::cmake_facade& cmake_facade,
                           facade::visibility v,
                           const std::string& target_name) const
{
  cmake_facade.target_link_library(m_name, v, target_name);
}
}
