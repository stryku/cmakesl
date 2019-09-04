#include "exec/cross_translation_unit_static_variables.hpp"

#include "common/algorithm.hpp"
#include "exec/execution.hpp"
#include "exec/module_sema_tree_provider.hpp"
#include "exec/static_variables_initializer.hpp"

namespace cmsl::exec {
cross_translation_unit_static_variables::
  cross_translation_unit_static_variables(
    facade::cmake_facade& cmake_facade,
    sema::builtin_types_accessor builtin_types,
    module_sema_tree_provider& sema_tree_provider)
  : m_cmake_facade{ cmake_facade }
  , m_builtin_types{ builtin_types }
  , m_sema_tree_provider{ sema_tree_provider }
{
}

cross_translation_unit_static_variables::
  ~cross_translation_unit_static_variables()
{
}

void cross_translation_unit_static_variables::initialize_module(
  const sema::sema_node& module_sema_tree)
{
  execution e{ m_cmake_facade, m_builtin_types, *this };
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
  const std::vector<sema::identifier_info>& builtin_variables_info)
{
  inst::instances_holder instances{ m_builtin_types };
  for (const auto& info : builtin_variables_info) {
    const auto created = instances.create(info.type);
    m_variables[info.index] = instances.gather_ownership(created);
  }
}
}