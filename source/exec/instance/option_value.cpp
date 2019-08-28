#include "exec/instance/option_value.hpp"

namespace cmsl::exec::inst {
option_value::option_value(const std::string& description, bool value)
  : m_description{ description }
  , m_value{ value }
{
}

const std::string& option_value::description() const
{
  return m_description;
}

bool option_value::value() const
{
  return m_value;
}
}