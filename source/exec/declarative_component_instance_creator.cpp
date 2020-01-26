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
  inst::instances_holder_interface& instances,
  const sema::generic_type_creation_utils& generic_types,
  errors::errors_observer& errors_observer)
  : m_facade{ facade }
  , m_builtin_types{ builtin_types }
  , m_decl_types{ decl_types }
  , m_instances{ instances }
  , m_generic_types{ generic_types }
  , m_errs{ errors_observer }
{
}

std::unique_ptr<inst::instance> declarative_component_instance_creator::create(
  const decl_sema::component_creation_sema_function& function)
{
  const auto& lib_type = function.return_type();
  auto lib_instance = m_instances.create(lib_type);

  declarative_component_property_instances_collecting_visitor collector{
    m_facade,        m_builtin_types, m_instances,
    m_generic_types, *lib_instance,   m_errs
  };

  function.component().visit(collector);

  register_in_facade(*lib_instance);

  return m_instances.gather_ownership(lib_instance);
}

void declarative_component_instance_creator::register_in_facade(
  const inst::instance& instance)
{
  const auto& inst_type = instance.type();

  const auto name_info = inst_type.find_member("name");
  CMSL_ASSERT(name_info.has_value());
  const auto name_instance = instance.find_cmember(name_info->index);

  const auto name_prefix_info = inst_type.find_member("name_prefix");
  CMSL_ASSERT(name_prefix_info.has_value());
  const auto name_prefix_instance =
    instance.find_cmember(name_prefix_info->index);

  const auto name_suffix_info = inst_type.find_member("name_suffix");
  CMSL_ASSERT(name_suffix_info.has_value());
  const auto name_suffix_instance =
    instance.find_cmember(name_suffix_info->index);

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

  const auto name_str = name_prefix_instance->value_cref().get_string_cref() +
    name_instance->value_cref().get_string_cref() +
    name_suffix_instance->value_cref().get_string_cref();

  if (instance.type() == m_decl_types.static_library ||
      instance.type().derives_from(m_decl_types.static_library)) {
    m_facade.add_library(name_str, sources);
    register_in_facade<inst::library_value>(name_str, instance);
  } else if (instance.type() == m_decl_types.executable ||
             instance.type().derives_from(m_decl_types.executable)) {

    m_facade.add_executable(name_str, sources);
    register_in_facade<inst::executable_value>(name_str, instance);
  } else if (instance.type() == m_decl_types.test_executable ||
             instance.type().derives_from(m_decl_types.test_executable)) {

    m_facade.add_executable(name_str, sources);
    m_facade.add_test(name_str);
    register_in_facade<inst::executable_value>(name_str, instance);
  }
}

template <typename ValueType>
void declarative_component_instance_creator::register_in_facade(
  const std::string& name, const inst::instance& instance)
{
  ValueType val{ name };

  {
    const auto register_callback =
      [&val, this](const auto& list_value, facade::visibility visibility) {
        val.include_directories(m_facade, visibility, list_value);
      };
    register_forwarding_lists(instance, "include_dirs", register_callback);
  }
  {
    const auto register_callback =
      [&val, this](const auto& list_value, facade::visibility visibility) {
        val.compile_options(m_facade, list_value, visibility);
      };
    register_forwarding_lists(instance, "compile_options", register_callback);
  }
  {
    const auto register_callback =
      [&val, this](const auto& list_value, facade::visibility visibility) {
        val.compile_definitions(m_facade, list_value, visibility);
      };
    register_forwarding_lists(instance, "compile_definitions",
                              register_callback);
  }
  {
    const auto register_callback =
      [&val, this](const auto& list_value, facade::visibility visibility) {
        val.add_sources(m_facade, list_value, visibility);
      };
    register_forwarding_lists(instance, "files", register_callback);
  }
  {
    const auto register_callback =
      [&val, this](const auto& list_value, facade::visibility visibility) {
        const auto dependencies =
          inst::list_value_utils{ list_value }.strings();
        for (const auto& dep : dependencies) {
          val.link_to(m_facade, visibility, dep);
        }
      };
    register_forwarding_lists(instance, "dependencies", register_callback);
  }
}

template <typename RegisterCallback>
void declarative_component_instance_creator::register_forwarding_lists(
  const inst::instance& component_instance, cmsl::string_view lists_name,
  RegisterCallback&& register_callback)
{
  const auto& inst_type = component_instance.type();

  const auto public_info = m_decl_types.forwarding_lists.find_member("public");
  const auto private_info =
    m_decl_types.forwarding_lists.find_member("private");
  const auto interface_info =
    m_decl_types.forwarding_lists.find_member("interface");

  const auto lists_info = inst_type.find_member(lists_name);
  CMSL_ASSERT(lists_info.has_value());
  const auto lists_instance =
    component_instance.find_cmember(lists_info->index);

  const auto pairs = {
    std::pair{ public_info, facade::visibility::public_ },
    std::pair{ private_info, facade::visibility::private_ },
    std::pair{ interface_info, facade::visibility::interface },
  };

  for (const auto& [info, visibility] : pairs) {
    const auto visibility_based_list_instance =
      lists_instance->find_cmember(info->index);
    CMSL_ASSERT(visibility_based_list_instance);

    const auto& visibility_based_list_value =
      visibility_based_list_instance->value_cref().get_list_cref();
    register_callback(visibility_based_list_value, visibility);
  }
}
}
