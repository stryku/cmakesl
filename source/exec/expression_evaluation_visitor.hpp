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
class expression_evaluation_visitor : public sema::empty_sema_node_visitor
{
public:
  explicit expression_evaluation_visitor(expression_evaluation_context& ctx)
    : m_ctx{ ctx }
  {
  }

  void visit(const sema::bool_value_node& node) override
  {
    result = m_ctx.instances.create(node.value());
  }

  void visit(const sema::int_value_node& node) override
  {
    result = m_ctx.instances.create(node.value());
  }

  void visit(const sema::double_value_node& node) override
  {
    result = m_ctx.instances.create(node.value());
  }

  void visit(const sema::string_value_node& node) override
  {
    result = m_ctx.instances.create(std::string{ node.value() });
  }

  void visit(const sema::id_node& node) override
  {
    const auto str = node.names().back().name.str();
    result = m_ctx.ids_context.lookup_identifier(node.index());
  }

  void visit(const sema::binary_operator_node& node) override
  {
    auto lhs_result = evaluate_child(node.lhs());
    auto rhs_result = evaluate_child(node.rhs());
    const auto& operator_function = node.operator_function();

    // Todo: use small vector.
    std::vector<inst::instance*> params;
    params.emplace_back(rhs_result);
    auto result_instance = m_ctx.caller.call_member(
      *lhs_result, operator_function, std::move(params), m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::function_call_node& node) override
  {
    auto evaluated_params =
      evaluate_call_parameters(node.function(), node.param_expressions());
    const auto& function = node.function();
    auto result_instance =
      m_ctx.caller.call(function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::add_subdirectory_node& node) override
  {
    m_ctx.cmake_facade.go_into_subdirectory(
      std::string{ node.dir_name().value() });

    auto evaluated_params =
      evaluate_call_parameters(node.function(), node.param_expressions());
    const auto& function = node.function();
    auto result_instance =
      m_ctx.caller.call(function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));

    m_ctx.cmake_facade.go_directory_up();
  }

  void visit(const sema::implicit_member_function_call_node& node) override
  {
    auto evaluated_params =
      evaluate_call_parameters(node.function(), node.param_expressions());
    const auto& function = node.function();
    auto class_instance = m_ctx.ids_context.get_class_instance();
    auto result_instance = m_ctx.caller.call_member(
      *class_instance, function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::constructor_call_node& node) override
  {
    auto evaluated_params =
      evaluate_call_parameters(node.function(), node.param_expressions());
    const auto& function = node.function();
    auto class_instance = m_ctx.instances.create(node.type());
    auto result_instance = m_ctx.caller.call_member(
      *class_instance, function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::member_function_call_node& node) override
  {
    auto lhs_result = evaluate_child(node.lhs());
    auto evaluated_params =
      evaluate_call_parameters(node.function(), node.param_expressions());
    const auto& function = node.function();
    auto result_instance = m_ctx.caller.call_member(
      *lhs_result, function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::class_member_access_node& node) override
  {
    auto lhs = evaluate_child(node.lhs());
    result = lhs->find_member(node.member_index());
  }

  void visit(const sema::return_node& node) override
  {
    result = evaluate_child(node.expression());
  }

  void visit(const sema::cast_to_reference_node& node) override
  {
    const auto evaluated = evaluate_child(node.expression());
    result = m_ctx.instances.create_reference(*evaluated);
  }

  void visit(const sema::cast_to_value_node& node) override
  {
    const auto evaluated = evaluate_child(node.expression());
    const auto& evaluated_reference_type = evaluated->type();
    const auto& referenced_type = evaluated_reference_type.referenced_type();
    result = m_ctx.instances.create(referenced_type, evaluated->value());
  }

  void visit(const sema::initializer_list_node& node) override
  {
    auto list_instance = m_ctx.instances.create(node.type());
    auto& list = list_instance->value_ref().get_list_ref();

    for (const auto& value_expression : node.values()) {
      const auto evaluated = evaluate_child(*value_expression);
      auto owned_value = m_ctx.instances.gather_ownership(evaluated);
      list.push_back(std::move(owned_value));
    }

    result = list_instance;
  }

  void visit(const sema::ternary_operator_node& node) override
  {
    const auto condition = evaluate_child(node.condition());
    const auto& condition_result_value = condition->value_cref();

    if (condition_result_value.get_bool()) {
      result = evaluate_child(node.true_());
    } else {
      result = evaluate_child(node.false_());
    }
  }

  void visit(const sema::designated_initializers_node& node) override
  {
    const auto& expected_type = m_ctx.expected_types.top().get();
    auto instance = m_ctx.instances.create(expected_type);

    for (const auto& initializer : node.initializers()) {
      const auto member_info =
        expected_type.find_member(initializer.name.str());
      auto guard = set_expected_type(member_info->ty);

      auto initialization_value = evaluate_child(*initializer.init);

      // It could probably be gathered from the instances.
      instance->assign_member(member_info->index,
                              initialization_value->copy());
    }

    result = instance;
  }

  void visit(const sema::unary_operator_node& node) override
  {
    auto lhs_result = evaluate_child(node.expression());
    const auto& operator_function = node.function();

    // Todo: use small vector.
    std::vector<inst::instance*> params;
    auto result_instance = m_ctx.caller.call_member(
      *lhs_result, operator_function, std::move(params), m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

public:
  inst::instance* result;

private:
  template <typename T>
  inst::instance* evaluate_child(const T& child)
  {
    auto c = clone();
    child.visit(c);
    return c.result;
  }

  std::vector<inst::instance*> evaluate_call_parameters(
    const sema::sema_function& function,
    const sema::call_node::param_expressions_t& params)
  {
    std::vector<inst::instance*> evaluated_params;

    for (auto i = 0; i < params.size(); ++i) {
      const auto& expected_type = function.signature().params[i].ty;
      auto guard = set_expected_type(expected_type);
      auto param = evaluate_child(*params[i]);
      evaluated_params.emplace_back(param);
    }

    return evaluated_params;
  }

  expression_evaluation_visitor clone()
  {
    return expression_evaluation_visitor{ m_ctx };
  }

  std::string operator_to_str(lexer::token op)
  {
    switch (op.get_type()) {
      case lexer::token_type::equal:
      case lexer::token_type::equalequal:
      case lexer::token_type::minus:
      case lexer::token_type::minusminus:
      case lexer::token_type::minusequal:
      case lexer::token_type::plus:
      case lexer::token_type::plusplus:
      case lexer::token_type::plusequal:
      case lexer::token_type::amp:
      case lexer::token_type::ampamp:
      case lexer::token_type::ampequal:
      case lexer::token_type::pipe:
      case lexer::token_type::pipepipe:
      case lexer::token_type::pipeequal:
      case lexer::token_type::slash:
      case lexer::token_type::slashequal:
      case lexer::token_type::star:
      case lexer::token_type::starequal:
      case lexer::token_type::percent:
      case lexer::token_type::percentequal:
      case lexer::token_type::exclaimequal:
      case lexer::token_type::xor_:
      case lexer::token_type::xorequal:
      case lexer::token_type::less:
      case lexer::token_type::lessequal:
      case lexer::token_type::greater:
      case lexer::token_type::greaterequal: {
        return std::string{ op.str() };
      }

      default:
        break;
    }

    CMSL_UNREACHABLE("Unknown operator token");
    return "";
  }

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

  expected_types_guard set_expected_type(const sema::sema_type& ty)
  {
    return expected_types_guard{ m_ctx.expected_types, ty };
  }

private:
  expression_evaluation_context& m_ctx;
};
}
