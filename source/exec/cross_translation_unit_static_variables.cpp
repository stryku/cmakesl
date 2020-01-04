#include "exec/cross_translation_unit_static_variables.hpp"

#include "common/algorithm.hpp"
#include "exec/builtin_identifiers_observer.hpp"
#include "exec/execution.hpp"
#include "exec/module_sema_tree_provider.hpp"
#include "exec/static_variables_initializer.hpp"
#include "sema/enum_type.hpp"

namespace cmsl::exec {
cross_translation_unit_static_variables::
  cross_translation_unit_static_variables(
    facade::cmake_facade& cmake_facade,
    sema::builtin_types_accessor builtin_types,
    decl_sema::decl_namespace_types_accessor decl_types,
    module_sema_tree_provider& sema_tree_provider,
    const sema::generic_type_creation_utils& generic_types)
  : m_cmake_facade{ cmake_facade }
  , m_builtin_types{ builtin_types }
  , m_decl_types{ decl_types }
  , m_sema_tree_provider{ sema_tree_provider }
  , m_generic_types{ generic_types }
{
}

cross_translation_unit_static_variables::
  ~cross_translation_unit_static_variables()
{
}

void cross_translation_unit_static_variables::initialize_module(
  const sema::sema_node& module_sema_tree)
{
  execution e{ m_cmake_facade, m_builtin_types, m_decl_types, *this,
               m_generic_types };
  static_variables_initializer initializer{
    e, m_builtin_types, m_cmake_facade,
    /*module_variables_initializer=*/*this
  };

  module_sema_tree.visit(initializer);
  auto instances = initializer.gather_instances();
  std::move(std::begin(instances), std::end(instances),
            std::inserter(m_variables, std::end(m_variables)));
}

inst::instance* cross_translation_unit_static_variables::access_variable(
  unsigned index)
{
  const auto found = m_variables.find(index);
  if (found == std::cend(m_variables)) {
    return nullptr;
  }

  return found->second.get();
}

void cross_translation_unit_static_variables::initialize(
  cmsl::string_view import_path)
{
  if (contains(m_already_initialized_modules, import_path)) {
    return;
  }

  const auto& tree = m_sema_tree_provider.get_sema_tree(import_path);
  initialize_module(tree);
}

void cross_translation_unit_static_variables::initialize_builtin_variables(
  const std::vector<sema::builtin_identifier_info>& builtin_variables_info,
  const builtin_identifiers_observer& observer)
{
  inst::instances_holder instances{ m_builtin_types };
  for (const auto& info : builtin_variables_info) {
    auto created_observer =
      create_observer(info.cmake_variable_name, observer);
    const auto created = instances.create_observable(
      info.id_info.type, std::move(created_observer));
    m_variables[info.id_info.index] = instances.gather_ownership(created);
  }
}

inst::instance_value_observer_t
cross_translation_unit_static_variables::create_observer(
  const std::string& variable_name,
  const builtin_identifiers_observer& observer)
{
  return
    [&observer, variable_name](const sema::sema_type& type,
                               const inst::instance_value_variant& value) {
      std::string result;

      switch (value.which()) {
        case inst::instance_value_alternative::bool_: {
          result = value.get_bool() ? "true" : "false";
        } break;
        case inst::instance_value_alternative::int_: {
          result = std::to_string(value.get_int());
        } break;
        case inst::instance_value_alternative::double_: {
          result = std::to_string(value.get_double());
        } break;
        case inst::instance_value_alternative::enum_: {
          const auto& enum_type = static_cast<const sema::enum_type&>(type);
          result = enum_type.enumerator(value.get_enum_constant().value).str();
        } break;
        case inst::instance_value_alternative::string: {
          result = value.get_string_cref();
        } break;
        case inst::instance_value_alternative::version: {
        } break;
        case inst::instance_value_alternative::extern_: {
        } break;
        case inst::instance_value_alternative::list: {
        } break;
        case inst::instance_value_alternative::project: {
        } break;
        case inst::instance_value_alternative::library: {
        } break;
        case inst::instance_value_alternative::executable: {
        } break;
        case inst::instance_value_alternative::option: {
        } break;
      }

      observer.notify(variable_name, result);
    };
}
}