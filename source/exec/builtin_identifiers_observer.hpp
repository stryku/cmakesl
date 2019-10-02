#pragma once

#include <string>

namespace cmsl {

namespace facade {
class cmake_facade;
}

namespace exec {
class builtin_identifiers_observer
{
public:
  explicit builtin_identifiers_observer(facade::cmake_facade& f);

  void notify(const std::string& variable_name,
              const std::string& value) const;

private:
  facade::cmake_facade& m_facade;
};
}
}
