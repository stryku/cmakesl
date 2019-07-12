#include "exec/execution_context.hpp"

#include "common/assert.hpp"
#include "exec/instance/instance_reference.hpp"

namespace cmsl::exec {
void execution_context::add_variable(unsigned index,
                                     std::unique_ptr<instance_t> inst)
{
  current_scope().add_variable(index, std::move(inst));
}

execution_context::instance_t* execution_context::get_variable(unsigned index)
{
  return current_scope().get_variable(index);
}

bool execution_context::variable_exists(unsigned index) const
{
  return current_scope().variable_exists(index);
}

execution_context::scope_leaving_guard execution_context::enter_scope()
{
  auto current = m_scopes.empty() ? nullptr : &current_scope();
  m_scopes.push(scope_context{ current });
  return scope_leaving_guard{ m_scopes };
}

void execution_context::leave_scope()
{
  m_scopes.pop();
}

scope_context& execution_context::current_scope()
{
  if (m_scopes.empty()) {
    CMSL_UNREACHABLE("No scope");
  }
  return m_scopes.top();
}

const scope_context& execution_context::current_scope() const
{
  if (m_scopes.empty()) {
    CMSL_UNREACHABLE("No scope");
  }
  return m_scopes.top();
}

execution_context::scope_leaving_guard
execution_context::enter_member_function_scope(instance_t* class_instance)
{
  m_scopes.push(scope_context{ class_instance });
  add_variable(k_this_index,
               std::make_unique<inst::instance_reference>(*class_instance));
  return scope_leaving_guard{ m_scopes };
}

execution_context::instance_t* execution_context::get_this()
{
  return get_variable(k_this_index);
}
}
