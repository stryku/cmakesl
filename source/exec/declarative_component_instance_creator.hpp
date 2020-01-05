#pragma once

#include "decl_sema/decl_namespace_types_accessor.hpp"
#include "sema/builtin_types_accessor.hpp"

#include <memory>

namespace cmsl {
namespace decl_sema {
class component_creation_sema_function;
class property_access_node;
struct decl_namespace_types_accessor;
}

namespace facade {
class cmake_facade;
}

namespace sema {
class factories_provider;
class generic_type_creation_utils;
}

namespace exec {
namespace inst {
class instance;
class instances_holder_interface;
}

class declarative_component_instance_creator
{
public:
  explicit declarative_component_instance_creator(
    facade::cmake_facade& facade, sema::builtin_types_accessor builtin_types,
    decl_sema::decl_namespace_types_accessor decl_types,
    inst::instances_holder_interface& instances,
    const sema::generic_type_creation_utils& generic_types);

  std::unique_ptr<inst::instance> create(
    const decl_sema::component_creation_sema_function& function);

private:
  inst::instance& access_property_instance(
    inst::instance& component_instance,
    const decl_sema::property_access_node& node);

  void register_in_facade(const inst::instance& instance);

  template <typename ValueType>
  void register_in_facade(const std::string& name,
                          const inst::instance& instance);

private:
  facade::cmake_facade& m_facade;
  const sema::builtin_types_accessor m_builtin_types;
  const decl_sema::decl_namespace_types_accessor m_decl_types;
  inst::instances_holder_interface& m_instances;
  const sema::generic_type_creation_utils& m_generic_types;
};
}
}
