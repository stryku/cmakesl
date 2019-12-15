#pragma once

#include "visibility.hpp"

#include <string>

namespace cmsl {
namespace facade {
class cmake_facade;
}

namespace exec::inst {
class list_value;

class target_value
{
public:
  explicit target_value(std::string name);

  std::string name() const;

  void link_to(facade::cmake_facade& cmake_facade, facade::visibility v,
               const target_value& target) const;

  void link_to(facade::cmake_facade& cmake_facade, facade::visibility v,
               const std::string& target_name) const;

  void include_directories(facade::cmake_facade& cmake_facade,
                           facade::visibility v, const list_value& dirs) const;

  void compile_definitions(facade::cmake_facade& cmake_facade,
                           const list_value& definitions,
                           facade::visibility v) const;

private:
  std::string m_name;
};

class library_value : public target_value
{
public:
  using target_value::target_value;
};

class executable_value : public target_value
{
public:
  using target_value::target_value;
};
}
}
