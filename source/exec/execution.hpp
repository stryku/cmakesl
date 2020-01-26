#pragma once

#include "decl_sema/decl_namespace_types_accessor.hpp"
#include "exec/builtin_function_caller.hpp"
#include "exec/expression_evaluation_context.hpp"
#include "exec/expression_evaluation_visitor.hpp"
#include "exec/function_caller.hpp"
#include "exec/identifiers_context.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instance_factory.hpp"
#include "exec/instance/instances_holder.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/identifier_info.hpp"
#include "sema/sema_nodes.hpp"
#include "sema/user_sema_function.hpp"

#include "cmake_facade.hpp"

namespace cmsl {
namespace decl_sema {
class component_creation_sema_function;
struct decl_namespace_types_accessor;
}

namespace errors {
class errors_observer;
}

namespace sema {
class generic_type_creation_utils;
}

namespace exec {
class cross_translation_unit_static_variables_accessor;
class module_sema_tree_provider;
class module_static_variables_initializer;

class execution
  : public identifiers_context
  , public function_caller
{
public:
  explicit execution(facade::cmake_facade& cmake_facade,
                     sema::builtin_types_accessor builtin_types,
                     decl_sema::decl_namespace_types_accessor decl_types,
                     cross_translation_unit_static_variables_accessor&
                       static_variables_accessor,
                     const sema::generic_type_creation_utils& generic_types,
                     errors::errors_observer& errors_observer);

  void initialize_static_variables(
    const sema::translation_unit_node& node,
    module_static_variables_initializer& module_statics_initializer);

  // Todo: consider returning a reference
  // Todo: Call parameters should be a vector of std::unique_ptrs, not raw
  // pointers. Thanks to that, we'd avoid unnecessary instances copies.
  std::unique_ptr<inst::instance> call(
    const sema::sema_function& fun, const std::vector<inst::instance*>& params,
    inst::instances_holder_interface& instances) override;

  // Todo: consider returning a reference
  std::unique_ptr<inst::instance> call_member(
    inst::instance& class_instance, const sema::sema_function& fun,
    const std::vector<inst::instance*>& params,
    inst::instances_holder_interface& instances) override;

  inst::instance* lookup_identifier(unsigned index) override;
  inst::instance* get_class_instance() override;

private:
  void execute_block(const sema::block_node& block);
  void execute_variable_declaration(
    const sema::variable_declaration_node& node);
  void execute_node(const sema::sema_node& node);
  void execute_if_else_node(const sema::if_else_node& node);
  void execute_while_node(const sema::while_node& node);
  void execute_for_node(const sema::for_node& node);
  void execute_break_node(const sema::break_node& node);
  void execute_add_subdirectory_with_old_script(
    const sema::add_subdirectory_with_old_script_node& node);

  bool returning_from_function() const;

  const sema::sema_context& current_context() const;

  void enter_function_scope(const sema::sema_function& fun,
                            const std::vector<inst::instance*>& params);
  void enter_function_scope(const sema::sema_function& fun,
                            inst::instance& class_instance,
                            const std::vector<inst::instance*>& params);
  void leave_function_scope();

  std::unique_ptr<inst::instance> execute_infix_expression(
    expression_evaluation_context ctx, const sema::sema_node& node);

  std::unique_ptr<inst::instance> execute_infix_expression(
    const sema::sema_node& node);

  std::unique_ptr<inst::instance> execute_infix_expression(
    const sema::sema_type& expected_type, const sema::sema_node& node);

  std::unique_ptr<inst::instance> handle_component_creation_function(
    const decl_sema::component_creation_sema_function& function);

private:
  struct callstack_frame
  {
    const sema::sema_function& fun;
    execution_context exec_ctx{};
  };

  facade::cmake_facade& m_cmake_facade;
  sema::builtin_types_accessor m_builtin_types;
  decl_sema::decl_namespace_types_accessor m_decl_types;
  cross_translation_unit_static_variables_accessor&
    m_static_variables_accessor;
  const sema::generic_type_creation_utils& m_generic_types;
  errors::errors_observer& m_errs;

  std::unique_ptr<inst::instance> m_function_return_value;
  std::stack<callstack_frame> m_callstack;
  std::unordered_map<unsigned, std::unique_ptr<inst::instance>>
    m_global_variables;
  bool m_breaking_from_loop{ false };
};
}
}