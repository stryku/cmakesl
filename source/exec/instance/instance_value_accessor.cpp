#include "exec/instance/instance_value_accessor.hpp"
#include "exec/instance/instance_value_variant.hpp"

namespace cmsl::exec::inst {

instance_value_accessor::instance_value_accessor(
  const sema::sema_type& type, instance_value_variant& value_ref,
  instance_value_observer_t observer)
  : m_type{ type }
  , m_value_ref{ value_ref }
  , m_observer{ std::move(observer) }
{
}

instance_value_accessor::~instance_value_accessor()
{
  if (m_possibly_dirty) {
    call_observer();
  }
}

void instance_value_accessor::set_value(instance_value_variant new_value)
{
  m_value_ref = std::move(new_value);
}

instance_value_variant& instance_value_accessor::access()
{
  m_possibly_dirty = true;
  return m_value_ref;
}

void instance_value_accessor::call_observer() const
{
  if (m_observer) {
    m_observer(m_type, m_value_ref);
  }
}
}
