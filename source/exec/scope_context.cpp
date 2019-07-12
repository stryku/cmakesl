#include "exec/scope_context.hpp"

#include "common/algorithm.hpp"

namespace cmsl::exec {
scope_context::scope_context(scope_context* parent)
  : m_parent{ parent }
  , m_class_instance{ nullptr }
{
}

scope_context::scope_context(instance_t* class_instance)
  : m_parent{ nullptr }
  , m_class_instance{ class_instance }
{
}

void scope_context::add_variable(unsigned index,
                                 std::unique_ptr<inst::instance> inst)
{
  m_variables[index] = std::move(inst);
}

inst::instance* scope_context::get_variable(unsigned index)
{
  const auto it = m_variables.find(index);
  if (it != m_variables.cend()) {
    return it->second.get();
  } else if (m_parent != nullptr) {
    return m_parent->get_variable(index);
  } else if (m_class_instance != nullptr) {
    return m_class_instance->find_member(index);
  }

  return nullptr;
}

const inst::instance* scope_context::get_variable(unsigned index) const
{
  const auto it = m_variables.find(index);
  if (it != m_variables.cend()) {
    return it->second.get();
  } else if (m_parent != nullptr) {
    return m_parent->get_variable(index);
  }

  return nullptr;
}

bool scope_context::variable_exists(unsigned index) const
{
  return get_variable(index) != nullptr;
}
}
