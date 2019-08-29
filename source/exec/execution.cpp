#include "exec/execution.hpp"
#include "exec/static_variables_initializer.hpp"

namespace cmsl::exec {
execution::execution(facade::cmake_facade& cmake_facade,
                     sema::builtin_types_accessor builtin_types)
  : m_cmake_facade{ cmake_facade }
  , m_builtin_types{ builtin_types }
{
}

std::unique_ptr<inst::instance> execution::call(
  const sema::sema_function& fun, const std::vector<inst::instance*>& params,
  inst::instances_holder_interface& instances)
{
  std::unique_ptr<inst::instance> result;
  if (auto user_function =
        dynamic_cast<const sema::user_sema_function*>(&fun)) {
    enter_function_scope(fun, params);
    execute_block(user_function->body());
    result = std::move(m_function_return_value);
    leave_function_scope();
  } else {
    auto builtin_function =
      dynamic_cast<const sema::builtin_sema_function*>(&fun);
    result = builtin_function_caller2{ m_cmake_facade, instances, }.call(
      builtin_function->kind(), params);
  }

  return result;
}

std::unique_ptr<inst::instance> execution::call_member(
  inst::instance& class_instance, const sema::sema_function& fun,
  const std::vector<inst::instance*>& params,
  inst::instances_holder_interface& instances)
{
  if (auto user_function =
        dynamic_cast<const sema::user_sema_function*>(&fun)) {
    enter_function_scope(fun, class_instance, params);
    execute_block(user_function->body());
    leave_function_scope();
    return std::move(m_function_return_value);
  } else {
    auto builtin_function =
      dynamic_cast<const sema::builtin_sema_function*>(&fun);
    return builtin_function_caller2{ m_cmake_facade, instances }.call_member(
      class_instance, builtin_function->kind(), params);
  }
}

inst::instance* execution::lookup_identifier(unsigned index)
{
  if (auto found = m_global_variables.find(index);
      found != std::cend(m_global_variables)) {
    return found->second.get();
  } else {
    return m_callstack.top().exec_ctx.get_variable(index);
  }
}

inst::instance* execution::get_class_instance()
{
  return m_callstack.top().exec_ctx.get_this();
}

void execution::execute_block(const sema::block_node& block)
{
  auto guard = m_callstack.top().exec_ctx.enter_scope();
  for (const auto& expr : block.nodes()) {
    execute_node(*expr);
    if (returning_from_function() || m_breaking_from_loop) {
      return;
    }
  }
}

void execution::execute_variable_declaration(
  const sema::variable_declaration_node& node)
{
  auto& exec_ctx = m_callstack.top().exec_ctx;
  inst::instances_holder instances{ m_builtin_types };
  std::unique_ptr<inst::instance> created_instance;

  if (auto initialization = node.initialization()) {
    created_instance = execute_infix_expression(node.type(), *initialization);
  } else {
    // Todo: create variable without instances holder
    auto variable_instance_ptr = instances.create(node.type());
    created_instance = instances.gather_ownership(variable_instance_ptr);
  }

  exec_ctx.add_variable(node.index(), std::move(created_instance));
}

void execution::execute_node(const sema::sema_node& node)
{
  // Todo: consider introducing a visitor for such execution, instead of
  // dynamic casts.
  if (auto ret_node = dynamic_cast<const sema::return_node*>(&node)) {
    m_function_return_value = execute_infix_expression(node);
  } else if (auto ret_node =
               dynamic_cast<const sema::implicit_return_node*>(&node)) {

    inst::instances_holder instances{ m_builtin_types };
    auto void_val = instances.create_void();
    m_function_return_value = instances.gather_ownership(void_val);
  } else if (auto var_decl =
               dynamic_cast<const sema::variable_declaration_node*>(&node)) {
    execute_variable_declaration(*var_decl);
  } else if (auto if_else = dynamic_cast<const sema::if_else_node*>(&node)) {
    execute_if_else_node(*if_else);
  } else if (auto while_ = dynamic_cast<const sema::while_node*>(&node)) {
    execute_while_node(*while_);
  } else if (auto for_ = dynamic_cast<const sema::for_node*>(&node)) {
    execute_for_node(*for_);
  } else if (auto break_ = dynamic_cast<const sema::break_node*>(&node)) {
    execute_break_node(*break_);
  } else if (auto block = dynamic_cast<const sema::block_node*>(&node)) {
    execute_block(*block);
  } else {
    // A stand alone infix expression.
    (void)execute_infix_expression(node);
  }
}

bool execution::returning_from_function() const
{
  return m_function_return_value != nullptr;
}

const sema::sema_context& execution::current_context() const
{
  const auto& current_function = m_callstack.top().fun;
  return current_function.context();
}

void execution::enter_function_scope(
  const sema::sema_function& fun, const std::vector<inst::instance*>& params)
{
  m_callstack.push(callstack_frame{ fun });
  auto guard = m_callstack.top().exec_ctx.enter_scope();
  // Scope is explicitly left in leave_function_scope() method.
  guard.dismiss();
  auto& exec_ctx = m_callstack.top().exec_ctx;
  const auto& declared_params = fun.signature().params;

  for (auto i = 0u; i < params.size(); ++i) {
    auto param = params[i]->copy();
    exec_ctx.add_variable(declared_params[i].index, std::move(param));
  }
}

void execution::enter_function_scope(
  const sema::sema_function& fun, inst::instance& class_instance,
  const std::vector<inst::instance*>& params)
{
  m_callstack.push(callstack_frame{ fun });
  auto guard =
    m_callstack.top().exec_ctx.enter_member_function_scope(&class_instance);
  // Scope is explicitly left in leave_function_scope() method.
  guard.dismiss();
  auto& exec_ctx = m_callstack.top().exec_ctx;
  const auto& declared_params = fun.signature().params;

  for (auto i = 0u; i < params.size(); ++i) {
    auto param = params[i]->copy();
    exec_ctx.add_variable(declared_params[i].index, std::move(param));
  }
}

void execution::leave_function_scope()
{
  m_callstack.top().exec_ctx.leave_scope();
  m_callstack.pop();
}

std::unique_ptr<inst::instance> execution::execute_infix_expression(
  expression_evaluation_context ctx, const sema::sema_node& node)
{
  expression_evaluation_visitor visitor{ ctx };
  node.visit(visitor);
  return visitor.result->copy(); // Todo: it'd be good to move instead of copy.
}

std::unique_ptr<inst::instance> execution::execute_infix_expression(
  const sema::sema_node& node)
{
  inst::instances_holder instances{ m_builtin_types };
  expression_evaluation_context ctx{ *this, instances, *this, m_cmake_facade };
  return execute_infix_expression(std::move(ctx), node);
}

std::unique_ptr<inst::instance> execution::execute_infix_expression(
  const sema::sema_type& expected_type, const sema::sema_node& node)
{
  inst::instances_holder instances{ m_builtin_types };
  expression_evaluation_context::expected_types_t types;
  types.push(expected_type);
  expression_evaluation_context ctx{ *this, instances, *this, m_cmake_facade,
                                     std::move(types) };
  return execute_infix_expression(std::move(ctx), node);
}

void execution::execute_if_else_node(const sema::if_else_node& node)
{
  for (const auto& if_ : node.ifs()) {
    const auto& condition = if_->get_condition();
    auto condition_value = execute_infix_expression(condition);

    if (condition_value->value().get_bool()) {
      execute_block(if_->get_body());
      return;
    }
  }

  if (const auto else_body = node.else_body()) {
    execute_block(*else_body);
  }
}

void execution::execute_while_node(const sema::while_node& node)
{
  const auto& condition = node.condition();
  const auto& body = node.body();
  auto condition_result = execute_infix_expression(condition);
  while (condition_result->value_cref().get_bool()) {
    execute_block(body);

    if (returning_from_function() || m_breaking_from_loop) {
      m_breaking_from_loop = false;
      break;
    }

    condition_result = execute_infix_expression(condition);
  }
}

void execution::execute_for_node(const sema::for_node& node)
{
  auto guard = m_callstack.top().exec_ctx.enter_scope();

  if (node.init()) {
    if (auto var_decl =
          dynamic_cast<const sema::variable_declaration_node*>(node.init())) {
      execute_variable_declaration(*var_decl);
    } else {
      execute_infix_expression(*node.init());
    }
  }

  std::unique_ptr<inst::instance> condition_result;

  while (true) {
    execute_block(node.body());
    if (returning_from_function()) {
      break;
    }
    if (returning_from_function() || m_breaking_from_loop) {
      m_breaking_from_loop = false;
      break;
    }

    if (node.iteration()) {
      execute_infix_expression(*node.iteration());
    }

    if (node.condition()) {
      condition_result = execute_infix_expression(*node.condition());
      if (!condition_result->value_cref().get_bool()) {
        break;
      }
    }
  }
}

void execution::execute_break_node(const sema::break_node& node)
{
  m_breaking_from_loop = true;
}

bool execution::initialize_static_variables(
  const sema::translation_unit_node& node)
{
  static_variables_initializer initializer{ *this, m_builtin_types,
                                            m_cmake_facade };
  node.visit(initializer);
  m_global_variables = initializer.gather_instances();
  return true;
}
}
