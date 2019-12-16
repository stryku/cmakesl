#include "exec/declarative_component_instance_creator.hpp"
#include "cmake_facade.hpp"
#include "common/assert.hpp"
#include "exec/declarative_component_property_instances_collecting_visitor.hpp"

#include <unordered_map>

namespace cmsl::exec {

declarative_component_instance_creator::declarative_component_instance_creator(
  facade::cmake_facade& facade, sema::builtin_types_accessor builtin_types,
  inst::instances_holder_interface& instances)
  : m_facade{ facade }
  , m_builtin_types{ builtin_types }
  , m_instances{ instances }
{
}

std::unique_ptr<inst::instance> declarative_component_instance_creator::create(
  const decl_sema::component_creation_sema_function& function)
{
  if (function.return_type() == m_builtin_types.cmake->library) {
    return create_library(function);
  }

  if (function.return_type() == m_builtin_types.cmake->executable) {
    return create_executable(function);
  }

  CMSL_UNREACHABLE("Unknown component type");
}

std::unique_ptr<inst::instance>
declarative_component_instance_creator::create_library(
  const decl_sema::component_creation_sema_function& function)
{
  declarative_component_property_instances_collecting_visitor collector{
    m_facade, m_builtin_types, m_instances
  };

  function.component().visit(collector);

  auto& properties = collector.m_collected_properties;

  const auto library_name_it = properties.find("name");
  if (library_name_it == std::cend(properties)) {
    // Todo raise error
    return nullptr;
  }

  const auto& library_name =
    library_name_it->second->value_cref().get_string_cref();

  const auto& sources = properties["files"]->value_cref().get_list_cref();
  const auto string_sources = inst::list_value_utils{ sources }.strings();

  m_facade.add_library(library_name, string_sources);

  auto library_val = inst::library_value{ library_name };

  if (const auto found = properties.find("include_dirs");
      found != std::cend(properties)) {
    const auto& instance = found->second;
    const auto& include_dirs = instance->value_cref().get_list_cref();
    library_val.include_directories(m_facade, facade::visibility::private_,
                                    include_dirs);
  }

  if (const auto found = properties.find("compile_options");
      found != std::cend(properties)) {
    const auto& instance = found->second;
    const auto& compile_options = instance->value_cref().get_list_cref();
    library_val.compile_definitions(m_facade, compile_options,
                                    facade::visibility::private_);
  }

  if (const auto found = properties.find("dependencies");
      found != std::cend(properties)) {
    const auto& instance = found->second;
    const auto& deps = instance->value_cref().get_list_cref();
    const auto string_deps = inst::list_value_utils{ deps }.strings();
    for (const auto& dep_name : string_deps) {
      library_val.link_to(m_facade, facade::visibility::private_, dep_name);
    }
  }

  auto library_instance = m_instances.create(std::move(library_val));
  return m_instances.gather_ownership(library_instance);
}

std::unique_ptr<inst::instance>
declarative_component_instance_creator::create_executable(
  const decl_sema::component_creation_sema_function& function)
{
  declarative_component_property_instances_collecting_visitor collector{
    m_facade, m_builtin_types, m_instances
  };

  function.component().visit(collector);

  auto& properties = collector.m_collected_properties;

  const auto library_name_it = properties.find("name");
  if (library_name_it == std::cend(properties)) {
    // Todo raise error
    return nullptr;
  }

  const auto& executable_name =
    library_name_it->second->value_cref().get_string_cref();

  const auto& sources = properties["files"]->value_cref().get_list_cref();
  const auto string_sources = inst::list_value_utils{ sources }.strings();

  m_facade.add_executable(executable_name, string_sources);

  auto executable_val = inst::executable_value{ executable_name };

  if (const auto found = properties.find("include_dirs");
      found != std::cend(properties)) {
    const auto& instance = found->second;
    const auto& include_dirs = instance->value_cref().get_list_cref();
    executable_val.include_directories(m_facade, facade::visibility::private_,
                                       include_dirs);
  }

  if (const auto found = properties.find("compile_options");
      found != std::cend(properties)) {
    const auto& instance = found->second;
    const auto& compile_options = instance->value_cref().get_list_cref();
    executable_val.compile_definitions(m_facade, compile_options,
                                       facade::visibility::private_);
  }

  if (const auto found = properties.find("dependencies");
      found != std::cend(properties)) {
    const auto& instance = found->second;
    const auto& deps = instance->value_cref().get_list_cref();
    const auto string_deps = inst::list_value_utils{ deps }.strings();
    for (const auto& dep_name : string_deps) {
      executable_val.link_to(m_facade, facade::visibility::private_, dep_name);
    }
  }

  auto executable_instance = m_instances.create(std::move(executable_val));
  return m_instances.gather_ownership(executable_instance);
}
}
