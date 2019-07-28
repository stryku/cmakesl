#pragma once

#include "common/string.hpp"
#include "exec/instance/instance.hpp"

#include <unordered_map>

namespace cmsl::exec {
class scope_context
{
private:
  using instance_t = inst::instance;

public:
  // Used when creating initial function scope or scope in already created
  // scope
  explicit scope_context(scope_context* parent);

  // Used when entering a method which is member of class_instance
  explicit scope_context(instance_t* class_instance);

  void add_variable(unsigned index, std::unique_ptr<instance_t> inst);
  instance_t* get_variable(unsigned index);
  const instance_t* get_variable(unsigned index) const;

  bool variable_exists(unsigned index) const;

private:
  std::unordered_map<unsigned, std::unique_ptr<instance_t>> m_variables;
  scope_context* m_parent;
  instance_t* m_class_instance;
};
}
