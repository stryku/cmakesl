#pragma once

#include "sema/builtin_types_accessor.hpp"

#include <memory>

namespace cmsl {
namespace decl_sema {
class component_creation_sema_function;
}

namespace facade {
class cmake_facade;
}

namespace sema {
class factories_provider;
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
    inst::instances_holder_interface& instances);

  std::unique_ptr<inst::instance> create(
    const decl_sema::component_creation_sema_function& function);

private:
  std::unique_ptr<inst::instance> create_library(
    const decl_sema::component_creation_sema_function& function);
  std::unique_ptr<inst::instance> create_executable(
    const decl_sema::component_creation_sema_function& function);

private:
  facade::cmake_facade& m_facade;
  const sema::builtin_types_accessor m_builtin_types;
  inst::instances_holder_interface& m_instances;
};
}
}