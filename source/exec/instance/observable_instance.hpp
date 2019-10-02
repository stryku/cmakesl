#pragma once

#include "exec/instance/instance_value_observer.hpp"
#include "exec/instance/simple_unnamed_instance.hpp"

namespace cmsl::exec {
class execution_context;

namespace inst {
class observable_instance : public instance
{
public:
  explicit observable_instance(const sema::sema_type& type,
                               instance_value_observer_t observer);

  explicit observable_instance(const sema::sema_type& type,
                               instance_value_variant&& value,
                               instance_value_observer_t observer);

  instance_value_variant value() const override;
  instance_value_accessor value_accessor() override;
  const instance_value_variant& value_cref() const override;

  void assign(instance_value_variant val) override;
  void assign(std::unique_ptr<instance> val) override;
  void assign_member(unsigned index, std::unique_ptr<instance> val) override;

  std::unique_ptr<instance> copy() const override;

  instance* find_member(unsigned index) override;
  const instance* find_cmember(unsigned index) const override;
  sema::single_scope_function_lookup_result_t find_function(
    lexer::token name) const override;

  const sema::sema_type& type() const override;

private:
  simple_unnamed_instance m_instance;
  instance_value_observer_t m_observer;
};
}
}
