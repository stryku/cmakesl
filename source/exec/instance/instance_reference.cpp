#include "instance_reference.hpp"

#include "exec/execution_context.hpp"
#include "lexer/token.hpp"

namespace cmsl::exec::inst {
instance_reference::instance_reference(cmsl::string_view name,
                                       execution_context& ctx)
  : instance_reference{ *ctx.get_variable(name) }
{
}

instance_reference::instance_reference(instance& referenced_instance)
  : m_instance{ referenced_instance }
{
}

instance_value_variant instance_reference::value() const
{
  return m_instance.value();
}

instance_value_variant& instance_reference::value_ref()
{
  return m_instance.value_ref();
}

const instance_value_variant& instance_reference::value_cref() const
{
  return m_instance.value_cref();
}

void instance_reference::assign(instance_value_variant val)
{
  m_instance.assign(val);
}

std::unique_ptr<instance> instance_reference::copy() const
{
  return std::make_unique<instance_reference>(m_instance);
}

instance* instance_reference::find_member(cmsl::string_view name)
{
  return m_instance.find_member(name);
}

const instance* instance_reference::find_cmember(cmsl::string_view name) const
{
  return m_instance.find_cmember(name);
}

sema::single_scope_function_lookup_result_t instance_reference::find_function(
  lexer::token name) const
{
  return m_instance.find_function(name);
}

const sema::sema_type& instance_reference::type() const
{
  return m_instance.type();
}
}
