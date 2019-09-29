#pragma once

#include "exec/instance/instance.hpp"

namespace cmsl::exec {
class execution_context;

namespace inst {
class instance_reference : public instance
{
public:
  instance_reference(unsigned index, execution_context& ctx);

  explicit instance_reference(instance& referenced_instance);

  instance_value_variant value() const override;

  instance_value_variant& value_ref() override;

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
  instance& m_instance;
};
}
}
