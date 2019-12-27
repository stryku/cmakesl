#include "exec/declarative_component_instance_creator.hpp"
#include "cmake_facade.hpp"
#include "common/assert.hpp"
#include "decl_sema/component_creation_sema_function.hpp"
#include "exec/declarative_component_property_instances_collecting_visitor.hpp"
#include "exec/instance/list_value_utils.hpp"

#include <unordered_map>

namespace cmsl::exec {

declarative_component_instance_creator::declarative_component_instance_creator(
  facade::cmake_facade& facade, sema::builtin_types_accessor builtin_types,
  decl_sema::decl_namespace_types_accessor decl_types,
  inst::instances_holder_interface& instances)
  : m_facade{ facade }
  , m_builtin_types{ builtin_types }
  , m_decl_types{ decl_types }
  , m_instances{ instances }
{
}

std::unique_ptr<inst::instance> declarative_component_instance_creator::create(
  const decl_sema::component_creation_sema_function& function)
{
  declarative_component_property_instances_collecting_visitor collector{
    m_facade, m_builtin_types, m_instances
  };

  function.component().visit(collector);

  auto& property_values = collector.m_collected_properties;

  const auto& lib_type = function.return_type();
  auto lib_instance = m_instances.create(lib_type);

  for (auto& [property_access, property_init_value] : property_values) {
    auto& property_instance =
      access_property_instance(*lib_instance, *property_access);
    property_instance.assign(std::move(property_init_value));
  }

  register_in_facade(*lib_instance);

  return m_instances.gather_ownership(lib_instance);
}

inst::instance&
declarative_component_instance_creator::access_property_instance(
  inst::instance& component_instance,
  const decl_sema::property_access_node& node)
{
  auto current_instance = &component_instance;
  for (const auto& property_info : node.properties_access()) {
    const auto found_member =
      current_instance->find_member(property_info.index);
    CMSL_ASSERT(found_member);

    current_instance = found_member;
  }

  return *current_instance;
}

void declarative_component_instance_creator::register_in_facade(
  const inst::instance& instance)
{
  const auto& inst_type = instance.type();

  const auto name_info = inst_type.find_member("name");
  CMSL_ASSERT(name_info.has_value());
  const auto name_instance = instance.find_cmember(name_info->index);

  const auto files_info = inst_type.find_member("files");
  CMSL_ASSERT(files_info.has_value());
  const auto files_instance = instance.find_cmember(files_info->index);

  const auto public_files_info =
    m_decl_types.forwarding_lists.find_member("public");

  const auto public_files_instance =
    files_instance->find_cmember(public_files_info->index);

  const auto sources =
    inst::list_value_utils{
      public_files_instance->value_cref().get_list_cref()
    }
      .strings();

  const auto name_str = name_instance->value_cref().get_string_cref();

  if (instance.type().name().to_string() == "static_library") {
    m_facade.add_library(name_str, sources);
    register_in_facade<inst::library_value>(name_str, instance);

  } else if (instance.type().name().to_string() == "executable") {
    m_facade.add_executable(name_str, sources);
    register_in_facade<inst::library_value>(name_str, instance);
  }
}

template <typename ValueType>
void declarative_component_instance_creator::register_in_facade(
  const std::string& name, const inst::instance& instance)
{
  const auto& inst_type = instance.type();

  const auto public_info = m_decl_types.forwarding_lists.find_member("public");

  const auto include_dirs_info = inst_type.find_member("include_dirs");
  CMSL_ASSERT(include_dirs_info.has_value());
  const auto include_dirs_instance =
    instance.find_cmember(include_dirs_info->index);
  const auto public_include_dirs_instance =
    include_dirs_instance->find_cmember(public_info->index);

  const auto compile_options_info = inst_type.find_member("compile_options");
  CMSL_ASSERT(compile_options_info.has_value());
  const auto compile_options_instance =
    instance.find_cmember(compile_options_info->index);
  const auto public_compile_options_instance =
    compile_options_instance->find_cmember(public_info->index);

  const auto dependencies_info = inst_type.find_member("dependencies");
  CMSL_ASSERT(dependencies_info.has_value());
  const auto dependencies_instance =
    instance.find_cmember(dependencies_info->index);
  const auto public_dependencies_instance =
    dependencies_instance->find_cmember(public_info->index);
  const auto dependencies =
    inst::list_value_utils{
      public_dependencies_instance->value_cref().get_list_cref()
    }
      .strings();

  ValueType val{ name };
  val.include_directories(
    m_facade, facade::visibility::public_,
    public_include_dirs_instance->value_cref().get_list_cref());
  val.compile_definitions(
    m_facade, public_compile_options_instance->value_cref().get_list_cref(),
    facade::visibility::public_);

  for (const auto& dep : dependencies) {
    val.link_to(m_facade, facade::visibility::public_, dep);
  }
}
}
