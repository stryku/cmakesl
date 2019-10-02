#include "exec/builtin_identifiers_observer.hpp"
#include "cmake_facade.hpp"

namespace cmsl::exec {
builtin_identifiers_observer::builtin_identifiers_observer(
  facade::cmake_facade& f)
  : m_facade{ f }
{
}

void builtin_identifiers_observer::notify(
  const std::string& variable_name, const std::string& value) const
{
  m_facade.set_property(variable_name, value);
}
}
