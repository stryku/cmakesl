#pragma once

#include "cmake_facade.hpp"
#include "common/assert.hpp"
#include "exec/execution_context.hpp"
#include "exec/expression_evaluation_context.hpp"
#include "exec/function_caller.hpp"
#include "exec/identifiers_context.hpp"
#include "exec/instance/instance.hpp"
#include "exec/instance/instances_holder.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_node_visitor.hpp"
#include "sema/sema_nodes.hpp"

#include <algorithm>

namespace cmsl::exec {
class expression_evaluation_visitor : public sema::sema_node_visitor
{
public:
  explicit expression_evaluation_visitor(expression_evaluation_context& ctx);

  void visit(const sema::add_subdirectory_with_old_script_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::block_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::break_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::class_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::conditional_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::enum_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::for_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::function_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::if_else_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::implicit_return_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::import_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::namespace_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::translation_unit_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::variable_declaration_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::while_node& node) override
  {
    // Do nothing.
  }

  void visit(const sema::add_declarative_file_node& node) override;
  void visit(const sema::add_subdirectory_node& node) override;
  void visit(
    const sema::add_subdirectory_with_declarative_script_node& node) override;
  void visit(const sema::binary_operator_node& node) override;
  void visit(const sema::bool_value_node& node) override;
  void visit(const sema::cast_to_base_value_node& node) override;
  void visit(const sema::cast_to_reference_node& node) override;
  void visit(const sema::cast_to_reference_to_base_node& node) override;
  void visit(const sema::cast_to_value_node& node) override;
  void visit(const sema::class_member_access_node& node) override;
  void visit(const sema::constructor_call_node& node) override;
  void visit(const sema::designated_initializers_node& node) override;
  void visit(const sema::double_value_node& node) override;
  void visit(const sema::enum_constant_access_node& node) override;
  void visit(const sema::function_call_node& node) override;
  void visit(const sema::id_node& node) override;
  void visit(const sema::implicit_member_function_call_node& node) override;
  void visit(const sema::initializer_list_node& node) override;
  void visit(const sema::int_value_node& node) override;
  void visit(const sema::member_function_call_node& node) override;
  void visit(const sema::return_node& node) override;
  void visit(const sema::string_value_node& node) override;
  void visit(const sema::ternary_operator_node& node) override;
  void visit(const sema::unary_operator_node& node) override;

public:
  inst::instance* result;

private:
  template <typename T>
  inst::instance* evaluate_child(const T& child);

  std::vector<inst::instance*> evaluate_call_parameters(
    const sema::sema_function& function,
    const sema::call_node::param_expressions_t& params);

  expression_evaluation_visitor clone();

  std::string operator_to_str(lexer::token op);

  class expected_types_guard
  {
  public:
    explicit expected_types_guard(
      expression_evaluation_context ::expected_types_t& types,
      const sema::sema_type& ty)
      : m_types{ types }
    {
      m_types.push(ty);
    }
    ~expected_types_guard()
    {
      if (m_valid) {
        m_types.pop();
      }
    }

    expected_types_guard() = delete;
    expected_types_guard(const expected_types_guard&) = delete;
    expected_types_guard& operator=(const expected_types_guard&) = delete;

    expected_types_guard(expected_types_guard&& other)
      : m_types{ other.m_types }
    {
      other.m_valid = false;
    }

    expected_types_guard& operator=(expected_types_guard&& other)
    {
      m_valid = true;
      other.m_valid = false;
      return *this;
    }

  private:
    expression_evaluation_context ::expected_types_t& m_types;
    bool m_valid{ true };
  };

  expected_types_guard set_expected_type(const sema::sema_type& ty);

private:
  expression_evaluation_context& m_ctx;
};
}
