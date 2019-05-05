#pragma once

#include "exec/instance/instance.hpp"

namespace cmsl::exec {
class execution_context;

namespace inst {
class instance_reference : public instance
{
public:
  instance_reference(cmsl::string_view name, execution_context& ctx);
  instance_reference(instance& referenced_instance);

  instance_value_variant value() const override;
  instance_value_variant& value_ref() override;
  const instance_value_variant& value_cref() const override;
  void assign(instance_value_variant val) override;
  std::unique_ptr<instance> copy() const override;
  instance* find_member(cmsl::string_view name) override;
  const instance* find_cmember(cmsl::string_view name) const override;
  sema::single_scope_function_lookup_result_t find_function(
    lexer::token name) const override;
  const sema::sema_type& type() const override;

private:
  instance& m_instance;
};
}
}
