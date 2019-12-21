#include "instances_holder.hpp"
#include "exec/instance/instance.hpp"
#include "instance_factory.hpp"

#include "common/assert.hpp"

#include <algorithm>

namespace cmsl::exec::inst {
instances_holder::instances_holder(sema::builtin_types_accessor builtin_types)
  : m_builtin_types{ builtin_types }
{
}

std::unique_ptr<instance> instances_holder::gather_ownership(
  inst::instance* instance_ptr)
{
  auto found = std::find_if(
    std::begin(m_instances), std::end(m_instances),
    [instance_ptr](const auto& i) { return i.get() == instance_ptr; });

  if (found == std::end(m_instances)) {
    CMSL_UNREACHABLE("Gathering instance that doesn't belong to this "
                     "instances holder");
    return nullptr;
  }

  auto ret = std::move(*found);
  m_instances.erase(found);
  return ret;
}

void instances_holder::store(std::unique_ptr<instance> i)
{
  m_instances.emplace_back(std::move(i));
}

inst::instance* instances_holder::create(instance_value_variant value)
{
  auto instance =
    instance_factory2{}.create(std::move(value), m_builtin_types);
  auto ptr = instance.get();
  m_instances.emplace_back(std::move(instance));
  return ptr;
}

inst::instance* instances_holder::create_reference(
  inst::instance& referenced_instance)
{
  auto instance = instance_factory2{}.create_reference(referenced_instance);
  auto ptr = instance.get();
  m_instances.emplace_back(std::move(instance));
  return ptr;
}

inst::instance* instances_holder::create_reference_to_base(
  inst::instance& referenced_instance, const sema::sema_type& base_type)
{
  auto instance = instance_factory2{}.create_reference_to_base(
    referenced_instance, base_type);
  auto ptr = instance.get();
  m_instances.emplace_back(std::move(instance));
  return ptr;
}

inst::instance* instances_holder::create(const sema::sema_type& type)
{
  auto instance = instance_factory2{}.create(type);
  auto ptr = instance.get();
  m_instances.emplace_back(std::move(instance));
  return ptr;
}

inst::instance* instances_holder::create_observable(
  const sema::sema_type& type, instance_value_observer_t observer)
{
  if (observer == nullptr) {
    return create(type);
  }

  auto instance =
    instance_factory2{}.create_observable(type, std::move(observer));
  auto ptr = instance.get();
  m_instances.emplace_back(std::move(instance));
  return ptr;
}

inst::instance* instances_holder::create_void()
{
  return create(true); // Todo: introduce void type
}

inst::instance* instances_holder::create(const sema::sema_type& type,
                                         instance_value_variant value)
{
  auto instance = instance_factory2{}.create(type, std::move(value));
  auto ptr = instance.get();
  m_instances.emplace_back(std::move(instance));
  return ptr;
}

bool instances_holder::owns(inst::instance* instance_ptr) const
{
  const auto found = std::find_if(
    std::cbegin(m_instances), std::cend(m_instances),
    [instance_ptr](const auto& i) { return i.get() == instance_ptr; });

  return found != std::cend(m_instances);
}
}
