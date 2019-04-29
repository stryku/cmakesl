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
    result = m_ctx.ids_context.lookup_identifier(node.id().str());
  }

  void visit(const sema::binary_operator_node& node) override
  {
    auto lhs_result = evaluate_child(node.lhs());
    auto rhs_result = evaluate_child(node.rhs());
    const auto& operator_function = node.operator_function();

    // Todo: use small vector.
    std::vector<inst::instance*> params;
    params.emplace_back(rhs_result);
    auto result_instance = m_ctx.function_caller.call_member(
      *lhs_result, operator_function, std::move(params), m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::function_call_node& node) override
  {
    auto evaluated_params = evaluate_call_parameters(node.param_expressions());
    const auto& function = node.function();
    auto result_instance =
      m_ctx.function_caller.call(function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::add_subdirectory_node& node) override
  {
    m_ctx.cmake_facade.go_into_subdirectory(
      std::string{ node.dir_name().value() });

    auto evaluated_params = evaluate_call_parameters(node.param_expressions());
    const auto& function = node.function();
    auto result_instance =
      m_ctx.function_caller.call(function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));

    m_ctx.cmake_facade.go_directory_up();
  }

  void visit(const sema::implicit_member_function_call_node& node) override
  {
    auto evaluated_params = evaluate_call_parameters(node.param_expressions());
    const auto& function = node.function();
    auto class_instance = m_ctx.ids_context.lookup_identifier(
      "this"); // Todo: it probably can be done better (to not search by
               // string)
    auto result_instance = m_ctx.function_caller.call_member(
      *class_instance, function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::constructor_call_node& node) override
  {
    auto evaluated_params = evaluate_call_parameters(node.param_expressions());
    const auto& function = node.function();
    auto class_instance = m_ctx.instances.create(node.type());
    auto result_instance = m_ctx.function_caller.call_member(
      *class_instance, function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::member_function_call_node& node) override
  {
    auto lhs_result = evaluate_child(node.lhs());
    auto evaluated_params = evaluate_call_parameters(node.param_expressions());
    const auto& function = node.function();
    auto result_instance = m_ctx.function_caller.call_member(
      *lhs_result, function, evaluated_params, m_ctx.instances);
    result = result_instance.get();
    m_ctx.instances.store(std::move(result_instance));
  }

  void visit(const sema::class_member_access_node& node) override
  {
    auto lhs = evaluate_child(node.lhs());
    result = lhs->find_member(node.member_name().str());
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
    const sema::call_node::param_expressions_t& params)
  {
    std::vector<inst::instance*> evaluated_params;

    std::transform(std::cbegin(params), std::cend(params),
                   std::back_inserter(evaluated_params),
                   [this](const auto& param_expression) {
                     return evaluate_child(*param_expression);
                   });

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

private:
  expression_evaluation_context& m_ctx;
};
}
