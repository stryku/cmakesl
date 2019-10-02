#include "exec/instance/observable_instance.hpp"
#include "exec/execution_context.hpp"
#include "lexer/token.hpp"

namespace cmsl::exec::inst {
observable_instance::observable_instance(const sema::sema_type& type,
                                         instance_value_observer_t observer)
  : m_instance{ type }
  , m_observer{ std::move(observer) }
{
}

observable_instance::observable_instance(const sema::sema_type& type,
                                         instance_value_variant&& value,
                                         instance_value_observer_t observer)
  : m_instance{ type, std::move(value) }
  , m_observer{ std::move(observer) }
{
}

instance_value_variant observable_instance::value() const
{
  return m_instance.value();
}

instance_value_accessor observable_instance::value_accessor()
{
  auto& value_ref = m_instance.value_accessor().access();
  return instance_value_accessor{ type(), value_ref, m_observer };
}

const instance_value_variant& observable_instance::value_cref() const
{
  return m_instance.value_cref();
}

void observable_instance::assign(instance_value_variant val)
{
  m_instance.assign(std::move(val));
}

void observable_instance::assign_member(unsigned index,
                                        std::unique_ptr<instance> val)
{
  m_instance.assign_member(index, std::move(val));
}

std::unique_ptr<instance> observable_instance::copy() const
{
  return std::make_unique<observable_instance>(
    type(), instance_value_variant{ value_cref() }, m_observer);
}

instance* observable_instance::find_member(unsigned index)
{
  return m_instance.find_member(index);
}

const instance* observable_instance::find_cmember(unsigned index) const
{
  return m_instance.find_cmember(index);
}

sema::single_scope_function_lookup_result_t observable_instance::find_function(
  lexer::token name) const
{
  return m_instance.find_function(name);
}

const sema::sema_type& observable_instance::type() const
{
  return m_instance.type();
}

void observable_instance::assign(std::unique_ptr<instance> val)
{
  m_instance.assign(std::move(val));
}
}
