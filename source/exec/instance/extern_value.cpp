#include "exec/instance/extern_value.hpp"
#include "exec/instance/instance.hpp"
#include "extern_value.hpp"

namespace cmsl::exec::inst {
extern_value::extern_value(std::unique_ptr<instance> value)
  : m_value{ std::move(value) }
{
}

extern_value::extern_value(const extern_value& other)
{
  m_value = other.m_value ? other.m_value->copy() : nullptr;
}

extern_value& extern_value::operator=(const extern_value& other)
{
  m_value = other.m_value ? other.m_value->copy() : nullptr;
  return *this;
}

extern_value::~extern_value()
{
}

bool extern_value::has_value() const
{
  return m_value != nullptr;
}

const instance& extern_value::value_cref() const
{

  return *m_value;
}
}
