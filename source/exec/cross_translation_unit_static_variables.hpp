#pragma once

#include "decl_sema/decl_namespace_types_accessor.hpp"
#include "exec/cross_translation_unit_static_variables_accessor.hpp"
#include "exec/instance/instance_value_observer.hpp"
#include "exec/module_static_variables_initializer.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/identifier_info.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace cmsl {
namespace errors {
class errors_observer;
}

namespace facade {
class cmake_facade;
}

namespace sema {
class sema_node;
class generic_type_creation_utils;
}

namespace exec {
namespace inst {
class instance;
class instance_value_variant;
}

class builtin_identifiers_observer;
class module_sema_tree_provider;

class cross_translation_unit_static_variables
  : public module_static_variables_initializer
  , public cross_translation_unit_static_variables_accessor
{
public:
  cross_translation_unit_static_variables(
    facade::cmake_facade& cmake_facade,
    sema::builtin_types_accessor builtin_types,
    decl_sema::decl_namespace_types_accessor decl_types,
    module_sema_tree_provider& sema_tree_provider,
    const sema::generic_type_creation_utils& generic_types,
    errors::errors_observer& errors_observer);

  ~cross_translation_unit_static_variables();

  void initialize_module(const sema::sema_node& module_sema_tree);
  void initialize(cmsl::string_view import_path);

  void initialize_builtin_variables(
    const std::vector<sema::builtin_identifier_info>& builtin_variables_info,
    const builtin_identifiers_observer& observer);

  inst::instance* access_variable(unsigned index);

private:
  inst::instance_value_observer_t create_observer(
    const std::string& variable_name,
    const builtin_identifiers_observer& observer);

private:
  facade::cmake_facade& m_cmake_facade;
  sema::builtin_types_accessor m_builtin_types;
  decl_sema::decl_namespace_types_accessor m_decl_types;
  module_sema_tree_provider& m_sema_tree_provider;
  const sema::generic_type_creation_utils& m_generic_types;
  errors::errors_observer& m_errs;

  std::unordered_set<std::string> m_already_initialized_modules;
  std::unordered_map<unsigned, std::unique_ptr<inst::instance>> m_variables;
};
}
}