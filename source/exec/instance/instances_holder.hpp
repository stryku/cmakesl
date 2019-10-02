#pragma once

#include "exec/instance/instance_value_observer.hpp"
#include "exec/instance/instances_holder_interface.hpp"
#include "sema/builtin_types_accessor.hpp"

#include <memory>
#include <vector>

namespace cmsl {
namespace sema {
class sema_context;
}

namespace exec {
class context_provider;

namespace inst {
class instance;

// class used to handle lifetime of 'rvalue' instances created during full
// expression execution
class instances_holder : public instances_holder_interface
{
public:
  explicit instances_holder(sema::builtin_types_accessor builtin_types);

  void store(std::unique_ptr<instance> i) override;

  std::unique_ptr<instance> gather_ownership(
    inst::instance* instance_ptr) override;
  bool owns(inst::instance* instance_ptr) const override;

  inst::instance* create(instance_value_variant value) override;
  inst::instance* create_reference(
    inst::instance& referenced_instance) override;
  inst::instance* create(const sema::sema_type& type) override;
  inst::instance* create_observable(const sema::sema_type& type,
                                    instance_value_observer_t observer);
  inst::instance* create(const sema::sema_type& type,
                         instance_value_variant value) override;
  inst::instance* create_void() override;

private:
  sema::builtin_types_accessor m_builtin_types;
  std::vector<std::unique_ptr<inst::instance>> m_instances;
};
}
}
}
