#pragma once

#include "exec/cross_translation_unit_static_variables_accessor.hpp"
#include "exec/module_static_variables_initializer.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/identifier_info.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace cmsl {
namespace facade {
class cmake_facade;
}

namespace sema {
class sema_node;
}

namespace exec {
namespace inst {
class instance;
}

class module_sema_tree_provider;

class cross_translation_unit_static_variables
  : public module_static_variables_initializer
  , public cross_translation_unit_static_variables_accessor
{
public:
  cross_translation_unit_static_variables(
    facade::cmake_facade& cmake_facade,
    sema::builtin_types_accessor builtin_types,
    module_sema_tree_provider& sema_tree_provider);

  ~cross_translation_unit_static_variables();

  void initialize_module(const sema::sema_node& module_sema_tree);
  void initialize(cmsl::string_view import_path);

  void initialize_builtin_variables(
    const std::vector<sema::identifier_info>& builtin_variables_info);

  inst::instance* access_variable(unsigned index);

private:
  facade::cmake_facade& m_cmake_facade;
  sema::builtin_types_accessor m_builtin_types;
  module_sema_tree_provider& m_sema_tree_provider;

  std::unordered_set<std::string> m_already_initialized_modules;
  std::unordered_map<unsigned, std::unique_ptr<inst::instance>> m_variables;
};
}
}