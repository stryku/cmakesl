#include "sema/sema_builder_ast_visitor.hpp"

#include "ast/block_node.hpp"
#include "ast/class_node.hpp"
#include "ast/conditional_node.hpp"
#include "ast/for_node.hpp"
#include "ast/if_else_node.hpp"
#include "ast/infix_nodes.hpp"
#include "ast/return_node.hpp"
#include "ast/translation_unit_node.hpp"
#include "ast/user_function_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "ast/while_node.hpp"
#include "common/algorithm.hpp"
#include "common/assert.hpp"
#include "errors/error.hpp"
#include "errors/errors_observer.hpp"
#include "sema/add_subdirectory_handler.hpp"
#include "sema/block_node_manipulator.hpp"
#include "sema/builtin_types_finder.hpp"
#include "sema/factories.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/overload_resolution.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_nodes.hpp"
#include "sema/sema_type.hpp"
#include "sema/type_builder.hpp"
#include "sema/user_sema_function.hpp"
#include "sema/variable_initialization_checker.hpp"

namespace cmsl::sema {
sema_builder_ast_visitor::ids_ctx_guard::ids_ctx_guard(
  identifiers_context& ids_context)
  : m_ids_ctx{ ids_context }
{
  m_ids_ctx.enter_ctx();
}

sema_builder_ast_visitor::ids_ctx_guard::~ids_ctx_guard()
{
  m_ids_ctx.leave_ctx();
}

sema_builder_ast_visitor::sema_builder_ast_visitor(
  sema_context& generic_types_context, sema_context& ctx,
  errors::errors_observer& errs, identifiers_context& ids_context,
  sema_type_factory& type_factory, sema_function_factory& function_factory,
  sema_context_factory& context_factory,
  add_subdirectory_handler& add_subdirectory_handler,
  const builtin_token_provider& builtin_token_provider,
  function_parsing_context& function_parsing_ctx)
  : m_generic_types_context{ generic_types_context }
  , m_ctx{ ctx }
  , m_errors_observer{ errs }
  , m_ids_context{ ids_context }
  , m_type_factory{ type_factory }
  , m_function_factory{ function_factory }
  , m_context_factory{ context_factory }
  , m_add_subdirectory_handler{ add_subdirectory_handler }
  , m_builtin_token_provider{ builtin_token_provider }
  , m_function_parsing_ctx{ function_parsing_ctx }
{
}

void sema_builder_ast_visitor::visit(const ast::block_node& node)
{
  auto ig = ids_guard();

  std::vector<std::unique_ptr<sema_node>> nodes;

  for (const auto& n : node.nodes()) {
    auto child = visit_child(*n);
    if (!child) {
      return;
    }

    nodes.emplace_back(std::move(child));
  }

  m_result_node = std::make_unique<block_node>(node, std::move(nodes));
}

void sema_builder_ast_visitor::visit(const ast::class_node& node)
{
  const auto name = node.name();
  const auto class_name_representation = ast::type_representation{ name };
  if (const auto found_type =
        m_ctx.find_type_in_this_scope(class_name_representation)) {
    raise_error(name, "Type redefinition");
    return;
  }

  auto class_ids_guard = ids_guard();

  auto& class_context = m_context_factory.create_class(&m_ctx);

  auto members =
    collect_class_members_and_add_functions_to_ctx(node, class_context);

  if (!members) {
    return;
  }

  // We move created ast ctx ownership but it will live for the whole program
  // lifetime, so it is safe to use class_ast_ctx raw pointer.
  auto& class_type = m_type_factory.create(
    class_context, class_name_representation, std::move(members->info));
  auto& class_type_reference = m_type_factory.create_reference(class_type);

  // Same as with ast context. class_type raw pointer will be valid. We move it
  // to context to make this class findable as a regular type (so e.g. inside
  // this class methods, the type will appear as a regular type).
  m_ctx.add_type(class_type);
  m_ctx.add_type(class_type_reference);

  std::vector<std::unique_ptr<function_node>> functions;

  for (auto function_declaration : members->functions) {
    m_function_parsing_ctx.function = function_declaration.fun;
    auto body = visit_child_node<block_node>(
      function_declaration.body_to_visit, class_context);
    m_function_parsing_ctx.function = nullptr;
    if (!body) {
      return;
    }

    function_declaration.fun->set_body(*body);
    functions.emplace_back(std::make_unique<function_node>(
      function_declaration.ast_function_node, *function_declaration.fun,
      std::move(body)));
  }

  m_result_node = std::make_unique<class_node>(
    node, name, std::move(members->declarations), std::move(functions));
}

void sema_builder_ast_visitor::visit(const ast::conditional_node& node)
{
  auto ig = ids_guard();

  auto condition = visit_child_expr(node.condition());
  if (!condition) {
    return;
  }

  auto body = visit_child_node<block_node>(node.body());
  if (!body) {
    return;
  }

  m_result_node = std::make_unique<conditional_node>(
    node, std::move(condition), std::move(body));
}

void sema_builder_ast_visitor::visit(const ast::if_else_node& node)
{
  std::vector<std::unique_ptr<conditional_node>> ifs;

  for (const auto& if_ : node.ifs()) {
    auto cond = visit_child_node<conditional_node>(*if_.conditional);
    if (!cond) {
      return;
    }

    ifs.emplace_back(std::move(cond));
  }

  std::unique_ptr<block_node> else_body;

  if (auto else_node = node.else_body()) {
    auto ig = ids_guard();

    else_body = visit_child_node<block_node>(*else_node);
    if (!else_body) {
      return;
    }
  }

  m_result_node =
    std::make_unique<if_else_node>(node, std::move(ifs), std::move(else_body));
}

void sema_builder_ast_visitor::visit(const ast::binary_operator_node& node)
{
  auto lhs = visit_child_expr(node.lhs());
  if (!lhs) {
    return;
  }

  // Todo: handle operators like ++ and ++(int)
  const auto op = node.operator_();
  auto lookup_result = lhs->type().find_member_function(op);
  if (lookup_result.empty()) {
    raise_error(op,
                lhs->type().name().to_string() + " doesn't support operator " +
                  std::string{ op.str() });
    return;
  }

  auto rhs = visit_child_expr(node.rhs());
  if (!rhs) {
    return;
  }

  overload_resolution over_resolution{ m_errors_observer, op };
  const auto chosen_function = over_resolution.choose(lookup_result, *rhs);
  if (!chosen_function) {
    return;
  }

  m_result_node = std::make_unique<binary_operator_node>(
    node, std::move(lhs), node.operator_(), *chosen_function, std::move(rhs),
    chosen_function->return_type());
}

void sema_builder_ast_visitor::visit(const ast::class_member_access_node& node)
{
  auto lhs = visit_child_expr(node.lhs());
  if (!lhs) {
    return;
  }

  const auto& lhs_type = lhs->type();
  const auto member_name = node.member_name();
  auto member_info = lhs_type.find_member(member_name.str());
  if (!member_info) {
    raise_error(member_name,
                lhs_type.name().to_string() + " does not have member " +
                  std::string{ member_name.str() });
    return;
  }

  m_result_node = std::make_unique<class_member_access_node>(
    node, std::move(lhs), *member_info);
}

std::unique_ptr<expression_node> sema_builder_ast_visitor::build_function_call(
  const ast::function_call_node& node)
{
  auto params = get_function_call_params(node.param_nodes());
  if (!params) {
    return nullptr;
  }

  const auto function_lookup_result = m_ctx.find_function(node.name());
  overload_resolution over_resolution{ m_errors_observer, node.name() };
  const auto chosen_function =
    over_resolution.choose(function_lookup_result, *params);
  if (!chosen_function) {
    return nullptr;
  }

  if (chosen_function == m_function_parsing_ctx.function) {
    const auto function_has_auto_return_type =
      m_function_parsing_ctx.function->try_return_type() == nullptr;
    if (function_has_auto_return_type) {
      const auto source = node.name().source();
      const auto range = node.name().src_range();
      raise_error(
        source, range,
        "recursively calling function with 'auto' return type is forbidden");
      return nullptr;
    }
  }

  // Convert call parameter nodes if need, e.g. to a cast_to_reference_node, if
  // function accepts a reference.
  params = convert_params_to_cast_nodes_if_need(chosen_function->signature(),
                                                std::move(*params));

  switch (chosen_function->context().type()) {
    case sema_context::context_type::namespace_: {
      return std::make_unique<function_call_node>(node, *chosen_function,
                                                  std::move(*params));
    } break;
    case sema_context::context_type::class_: {
      const auto is_constructor = chosen_function->signature().name.str() ==
        chosen_function->return_type().name().to_string();
      if (is_constructor) {
        return std::make_unique<constructor_call_node>(
          node, chosen_function->return_type(), *chosen_function,
          std::move(*params));
      } else {
        return std::make_unique<implicit_member_function_call_node>(
          node, *chosen_function, std::move(*params));
      }
    } break;

    default:
      CMSL_UNREACHABLE("Unknown context type");
  }

  return nullptr;
}

std::unique_ptr<expression_node>
sema_builder_ast_visitor::build_add_subdirectory_call(
  const ast::function_call_node& node)
{
  auto params_opt = get_function_call_params(node.param_nodes());
  if (!params_opt) {
    return nullptr;
  }

  auto& params = *params_opt;
  if (params.empty()) {
    raise_error(node.name(), "expected subdirectory name");
    return nullptr;
  }

  auto casted = dynamic_cast<string_value_node*>(params[0].get());
  if (!casted) {
    raise_error(node.name(), "expected string literal as a subdirectory name");
    return nullptr;
  }

  params[0].release();
  auto name_string_node = std::unique_ptr<string_value_node>(casted);
  const auto name = name_string_node->value();

  std::vector<std::unique_ptr<expression_node>> params_but_name;
  std::transform(std::next(std::begin(params)), std::end(params),
                 std::back_inserter(params_but_name),
                 [](auto& param) { return std::move(param); });

  const auto fun =
    m_add_subdirectory_handler.handle_add_subdirectory(name, params_but_name);
  if (!fun) {
    return nullptr;
  }

  return std::make_unique<add_subdirectory_node>(
    node, std::move(name_string_node), *fun, std::move(params_but_name));
}

void sema_builder_ast_visitor::visit(const ast::function_call_node& node)
{

  if (node.name().str() == "add_subdirectory") {
    m_result_node = build_add_subdirectory_call(node);
  } else {
    m_result_node = build_function_call(node);
  }
}

void sema_builder_ast_visitor::visit(
  const ast::member_function_call_node& node)
{
  const auto name = node.name();
  auto lhs = visit_child_expr(node.lhs());
  if (!lhs) {
    return;
  }

  auto params = get_function_call_params(node.param_nodes());
  if (!params) {
    return;
  }

  const auto member_functions = lhs->type().find_member_function(name);
  overload_resolution over_resolution{ m_errors_observer, node.name() };
  const auto chosen_function =
    over_resolution.choose(member_functions, *params);
  if (!chosen_function) {
    return;
  }

  // Convert call parameter nodes if need, e.g. to a cast_to_reference_node, if
  // function accepts a reference.
  params = convert_params_to_cast_nodes_if_need(chosen_function->signature(),
                                                std::move(*params));

  m_result_node = std::make_unique<member_function_call_node>(
    node, std::move(lhs), *chosen_function, std::move(*params));
}

void sema_builder_ast_visitor::visit(const ast::bool_value_node& node)
{
  const auto& type = builtin_types_finder{ m_ctx }.find_bool();
  const auto value = node.token().str() == "true";
  m_result_node = std::make_unique<bool_value_node>(node, type, value);
}

void sema_builder_ast_visitor::visit(const ast::int_value_node& node)
{
  const auto& type = builtin_types_finder{ m_ctx }.find_int();
  const auto token = node.token();
  char* end;
  const auto value =
    std::strtol(token.str().data(), &end, 10); // todo: handle hex etc
  m_result_node = std::make_unique<int_value_node>(node, type, value);
}

void sema_builder_ast_visitor::visit(const ast::double_value_node& node)
{
  const auto& type = builtin_types_finder{ m_ctx }.find_double();
  const auto token = node.token();
  char* end;
  const auto value =
    std::strtod(token.str().data(), &end); // todo: handle hex etc
  m_result_node = std::make_unique<double_value_node>(node, type, value);
}

void sema_builder_ast_visitor::visit(const ast::string_value_node& node)
{
  const auto& type = builtin_types_finder{ m_ctx }.find_string();

  // At this point node contains string value including "". We need to get rid
  // of them.
  const auto node_string = node.token().str();
  const auto string_without_quotation_marks =
    cmsl::string_view{ std::next(node_string.data()),
                       node_string.size() - 2u };

  m_result_node = std::make_unique<string_value_node>(
    node, type, string_without_quotation_marks);
}

void sema_builder_ast_visitor::visit(const ast::id_node& node)
{
  const auto id_token = node.get_identifier();
  const auto type = m_ids_context.type_of(node.get_identifier().str());

  if (!type) {
    raise_error(id_token,
                std::string{ id_token.str() } + " identifier not found");
    return;
  }

  m_result_node = std::make_unique<id_node>(node, *type, id_token);
}

void sema_builder_ast_visitor::visit(const ast::return_node& node)
{
  auto v = clone();
  node.expression().visit(v);
  if (!v.m_result_node) {
    return;
  }

  auto expr = to_expression(std::move(v.m_result_node));

  if (!check_function_return_type(*expr)) {
    return;
  }

  expr = convert_to_cast_return_node_if_need(std::move(expr));
  auto ret_node = std::make_unique<return_node>(node, std::move(expr));
  m_function_parsing_ctx.return_nodes.emplace_back(ret_node.get());
  m_result_node = std::move(ret_node);
}

void sema_builder_ast_visitor::visit(const ast::translation_unit_node& node)
{
  auto ig = ids_guard();
  std::vector<std::unique_ptr<sema_node>> nodes;

  for (const auto& n : node.nodes()) {
    auto visited_node = visit_child(*n);
    if (!visited_node) {
      return;
    }

    nodes.emplace_back(std::move(visited_node));
  }

  m_result_node =
    std::make_unique<translation_unit_node>(node, m_ctx, std::move(nodes));
}

void sema_builder_ast_visitor::visit(const ast::user_function_node& node)
{
  const auto& return_type_representation = node.return_type_representation();
  const auto should_deduce_return_type = return_type_representation.is_auto();

  const sema_type* return_type{ nullptr };

  if (!should_deduce_return_type) {
    return_type =
      try_get_or_create_generic_type(m_ctx, return_type_representation);
    if (!return_type) {
      return;
    }
  }

  // Todo: parameters need to be in the same ids context as function body.
  auto params_ids_guard = ids_guard();

  using param_decl_t = parameter_declaration;
  std::vector<param_decl_t> params;

  for (const auto& param_decl : node.param_declarations()) {
    auto param_type = try_get_or_create_generic_type(m_ctx, param_decl.ty);
    if (!param_type) {
      return;
    }

    params.emplace_back(param_decl_t{ *param_type, param_decl.name });
    m_ids_context.register_identifier(param_decl.name, *param_type);
  }

  function_signature signature{ node.name(), std::move(params) };
  auto& function =
    m_function_factory.create_user(m_ctx, return_type, std::move(signature));

  // Add function (without a body yet) to context, so it will be visible inside
  // function body in case of a recursive call.
  m_ctx.add_function(function);

  // Store pointer to function that is currently parsed,
  // so function body will be able to figure out function return type
  // and make casted return expression nodes as needed.
  m_function_parsing_ctx.function = &function;
  m_function_parsing_ctx.return_nodes.clear();
  auto block = visit_child_node<block_node>(node.body());
  m_function_parsing_ctx.function = nullptr;
  if (!block) {
    return;
  }

  if (should_deduce_return_type) {
    return_type = try_deduce_currently_parsed_function_return_type();
    if (!return_type) {
      return;
    }
    function.set_return_type(*return_type);
  }

  add_implicit_return_node_if_need(*block);

  // And set the body.
  function.set_body(*block);

  m_result_node =
    std::make_unique<function_node>(node, function, std::move(block));
}

void sema_builder_ast_visitor::visit(
  const ast::standalone_variable_declaration_node& node)
{
  node.variable_declaration().visit(*this);
}

void sema_builder_ast_visitor::visit(const ast::while_node& node)
{
  auto conditional = visit_child_node<conditional_node>(node.node());
  if (!conditional) {
    return;
  }

  m_result_node = std::make_unique<while_node>(node, std::move(conditional));
}

const sema_type* sema_builder_ast_visitor::try_get_or_create_generic_type(
  const sema_context& search_context, const ast::type_representation& name)
{
  const auto found = search_context.find_type(name);
  if (found) {
    return found;
  }

  if (!name.is_generic()) {
    raise_error(name.primary_name(), name.to_string() + " type not found.");
    return nullptr;
  }

  const auto proper_generic_token_types = { lexer::token_type ::kw_list };

  if (!contains(proper_generic_token_types, name.primary_name().get_type())) {
    raise_error(name.primary_name(),
                "'" + std::string{ name.primary_name().str() } +
                  "' is not a generic type.");
    return nullptr;
  }

  auto factory =
    sema_generic_type_factory{ m_generic_types_context, search_context,
                               m_type_factory,          m_function_factory,
                               m_context_factory,       m_errors_observer,
                               m_builtin_token_provider };

  return factory.create_generic(name);
}

template <typename T>
std::unique_ptr<T> sema_builder_ast_visitor::to_node(
  std::unique_ptr<sema_node> node) const
{
  return std::unique_ptr<T>(dynamic_cast<T*>(node.release()));
}

std::unique_ptr<expression_node> sema_builder_ast_visitor::to_expression(
  std::unique_ptr<sema_node> node) const
{
  return std::unique_ptr<expression_node>(
    dynamic_cast<expression_node*>(node.release()));
}

void sema_builder_ast_visitor::raise_note(const lexer::token token,
                                          const std::string& message)
{
  raise_note(token.source(), token.src_range(), message);
}

void sema_builder_ast_visitor::raise_note(const source_view& source,
                                          const source_range& src_range,
                                          const std::string& message)
{
  notify_error_observer(source, src_range, message, errors::error_type::note);
}

void sema_builder_ast_visitor::raise_error(const lexer::token token,
                                           const std::string& message)
{
  raise_error(token.source(), token.src_range(), message);
}

void sema_builder_ast_visitor::raise_error(const source_view& source,
                                           const source_range& src_range,
                                           const std::string& message)
{
  notify_error_observer(source, src_range, message, errors::error_type::error);
}

void sema_builder_ast_visitor::notify_error_observer(
  const source_view& source, const source_range& src_range,
  const std::string& message, errors::error_type err_type)
{
  const auto line_info = source.line(src_range.begin.line);
  const auto err =
    errors::error{ source.path(), line_info.line, line_info.start_pos,
                   message,       err_type,       src_range };
  m_errors_observer.nofify_error(err);
}

sema_builder_ast_visitor sema_builder_ast_visitor::clone() const
{
  return clone(m_ctx);
}

sema_builder_ast_visitor sema_builder_ast_visitor::clone(
  sema_context& ctx_to_visit) const
{
  return sema_builder_ast_visitor{
    m_generic_types_context,  ctx_to_visit,
    m_errors_observer,        m_ids_context,
    m_type_factory,           m_function_factory,
    m_context_factory,        m_add_subdirectory_handler,
    m_builtin_token_provider, m_function_parsing_ctx
  };
}

std::unique_ptr<expression_node> sema_builder_ast_visitor::visit_child_expr(
  const ast::ast_node& node)
{
  return to_node<expression_node>(visit_child(node));
}

template <typename T>
std::unique_ptr<T> sema_builder_ast_visitor::visit_child_node(
  const ast::ast_node& node)
{
  return to_node<T>(visit_child(node));
}

template <typename T>
std::unique_ptr<T> sema_builder_ast_visitor::visit_child_node(
  const ast::ast_node& node, sema_context& ctx_to_visit)
{
  return to_node<T>(visit_child(node, ctx_to_visit));
}

std::unique_ptr<sema_node> sema_builder_ast_visitor::visit_child(
  const ast::ast_node& node)
{
  auto v = clone();
  node.visit(v);
  return std::move(v.m_result_node);
}

std::unique_ptr<sema_node> sema_builder_ast_visitor::visit_child(
  const ast::ast_node& node, sema_context& ctx_to_visit)
{
  auto v = clone(ctx_to_visit);
  node.visit(v);
  return std::move(v.m_result_node);
}

std::optional<sema_builder_ast_visitor::param_expressions_t>
sema_builder_ast_visitor::get_function_call_params(
  const ast::call_node::params_t& passed_params)
{
  std::vector<std::unique_ptr<expression_node>> params;

  for (const auto& passed_param : passed_params) {
    auto param = visit_child_expr(*passed_param);
    if (!param) {
      return {};
    }

    params.emplace_back(std::move(param));
  }

  return std::move(params);
}

sema_builder_ast_visitor::ids_ctx_guard sema_builder_ast_visitor::ids_guard()
{
  return ids_ctx_guard{ m_ids_context };
}

std::optional<sema_builder_ast_visitor::function_declaration>
sema_builder_ast_visitor::get_function_declaration_and_add_to_ctx(
  const ast::user_function_node& node, sema_context_impl& ctx)
{
  const auto return_type_reference = node.return_type_representation();
  auto return_type =
    try_get_or_create_generic_type(ctx, return_type_reference);
  if (!return_type) {
    return {};
  }

  // Todo: parameters need to be in the same ids context as function body.
  auto params_ids_guard = ids_guard();

  std::vector<parameter_declaration> params;

  for (const auto& param_decl : node.param_declarations()) {
    auto param_type = try_get_or_create_generic_type(ctx, param_decl.ty);
    if (!param_type) {
      raise_error(param_decl.ty.primary_name(),
                  param_decl.ty.to_string() + " unknown parameter type");
      return {};
    }

    params.emplace_back(parameter_declaration{ *param_type, param_decl.name });
    m_ids_context.register_identifier(param_decl.name, *param_type);
  }

  function_signature signature{ node.name(), std::move(params) };
  auto& function =
    m_function_factory.create_user(ctx, return_type, std::move(signature));
  ctx.add_function(function);

  return function_declaration{ node, &function, node.body() };
}

std::optional<sema_builder_ast_visitor::class_members>
sema_builder_ast_visitor::collect_class_members_and_add_functions_to_ctx(
  const ast::class_node& node, sema_context_impl& class_context)
{
  class_members members;

  for (const auto& n : node.nodes()) {
    if (auto variable_decl =
          dynamic_cast<const ast::standalone_variable_declaration_node*>(
            n.get())) {
      auto member =
        visit_child_node<variable_declaration_node>(*variable_decl);
      if (!member) {
        return {};
      }

      members.info.emplace_back(member_info{ member->name(), member->type() });
      members.declarations.emplace_back(std::move(member));
    } else if (auto fun_node =
                 dynamic_cast<const ast::user_function_node*>(n.get())) {
      auto function_declaration =
        get_function_declaration_and_add_to_ctx(*fun_node, class_context);
      if (!function_declaration) {
        return {};
      }

      members.functions.emplace_back(*function_declaration);
    }
  }

  return std::move(members);
}

template <unsigned N>
lexer::token sema_builder_ast_visitor::make_token(lexer::token_type token_type,
                                                  const char (&tok)[N])
{
  // N counts also '\0'
  const auto src_range = source_range{ source_location{ 1u, 1u, 0u },
                                       source_location{ 1u, N, N - 1u } };
  return lexer::token{ token_type, src_range, source_view{ tok } };
}

std::unique_ptr<expression_node>
sema_builder_ast_visitor::convert_to_cast_return_node_if_need(
  std::unique_ptr<expression_node> expression)
{
  if (!m_function_parsing_ctx.function ||
      !m_function_parsing_ctx.function->try_return_type()) {
    return expression;
  }

  const auto& expected_result_type =
    m_function_parsing_ctx.function->return_type();
  return convert_to_cast_node_if_need(expected_result_type,
                                      std::move(expression));
}

std::unique_ptr<expression_node>
sema_builder_ast_visitor::convert_to_cast_node_if_need(
  const sema_type& expected_result_type,
  std::unique_ptr<expression_node> expression)
{
  if (expected_result_type == expression->type()) {
    return expression;
  }

  const auto& ast_node = expression->ast_node();

  if (expected_result_type.is_reference()) {
    return std::make_unique<cast_to_reference_node>(
      ast_node, expected_result_type, std::move(expression));
  } else {
    return std::make_unique<cast_to_value_node>(ast_node, expected_result_type,
                                                std::move(expression));
  }
}

sema_builder_ast_visitor::param_expressions_t
sema_builder_ast_visitor::convert_params_to_cast_nodes_if_need(
  const function_signature& signature, param_expressions_t params)
{
  for (auto i = 0u; i < params.size(); ++i) {
    const auto& declared_param_type = signature.params[i].ty;
    params[i] =
      convert_to_cast_node_if_need(declared_param_type, std::move(params[i]));
  }

  return params;
}

void sema_builder_ast_visitor::visit(const ast::initializer_list_node& node)
{
  if (node.values().empty()) {
    raise_error(node.open_brace(), "initializer lsit must contain values");
    return;
  }

  std::vector<std::unique_ptr<expression_node>> expression_values;
  const sema_type* value_type{ nullptr };

  for (const auto& value : node.values()) {
    auto expression = visit_child_expr(*value);
    if (!expression) {
      return;
    }

    const auto& expr_type = expression->type();
    if (value_type && *value_type != expr_type) {
      raise_error(node.open_brace(),
                  "all values of initializer list must have the same type");
      return;
    }

    if (!value_type) {
      value_type = &expr_type;
    }

    expression_values.emplace_back(std::move(expression));
  }

  // Todo: Introduce utility to create generic name representations
  using token_type_t = lexer::token_type;
  lexer::token_container_t tokens{ make_token(token_type_t::kw_list, "list"),
                                   make_token(token_type_t::less, "<") };
  const auto& value_type_tokens = value_type->name().tokens();
  tokens.insert(std::end(tokens), std::cbegin(value_type_tokens),
                std::cend(value_type_tokens));
  tokens.emplace_back(make_token(token_type_t::greater, ">"));

  const auto list_type_representation =
    ast::type_representation{ std::move(tokens), { value_type->name() } };

  auto type = try_get_or_create_generic_type(m_ctx, list_type_representation);
  if (!type) {
    // Should be impossible to get here.
    raise_error(node.open_brace(), "could not create initalizer_list type");
    return;
  }

  m_result_node = std::make_unique<initializer_list_node>(
    node, *type, std::move(expression_values));
}
void sema_builder_ast_visitor::add_implicit_return_node_if_need(
  block_node& block)
{
  if (is_last_node_return_node(block)) {
    return;
  }

  const auto& ast_node = block.ast_node();
  const auto& void_type = builtin_types_finder{ m_ctx }.find_void();
  auto ret_node = std::make_unique<implicit_return_node>(ast_node, void_type);

  auto manipulator = block_node_manipulator{ block };
  manipulator.append_expression(std::move(ret_node));
}
bool sema_builder_ast_visitor::is_last_node_return_node(
  const block_node& block) const
{
  const auto& nodes = block.nodes();
  if (nodes.empty()) {
    return false;
  }

  const auto& last_node = nodes.back();
  const auto last_node_is_return =
    dynamic_cast<const return_node*>(last_node.get()) != nullptr;

  return last_node_is_return;
}

bool sema_builder_ast_visitor::check_function_return_type(
  const expression_node& return_expression)
{
  if (m_function_parsing_ctx.function == nullptr) {
    CMSL_UNREACHABLE("Return statement outside of function");
  }

  // If currently parsed function has 'auto' return type, return true.
  // Return types are going to be check later.
  const auto function_has_auto_return_type =
    m_function_parsing_ctx.function->try_return_type() == nullptr;
  if (function_has_auto_return_type) {
    return true;
  }

  const auto src_range = return_expression.ast_node().src_range();
  const auto source =
    m_function_parsing_ctx.function->signature().name.source();

  const auto& function_return_type =
    m_function_parsing_ctx.function->return_type();

  variable_initialization_checker checker;
  const auto check_result =
    checker.check(function_return_type, return_expression);

  if (check_result ==
      variable_initialization_checker::check_result::can_init) {
    return true;
  }

  switch (check_result) {
    case variable_initialization_checker::check_result::different_types: {
      raise_error(source, src_range,
                  "Function return type and expression result type do not "
                  "match. Expected \'" +
                    function_return_type.name().to_string() + "\' got \'" +
                    return_expression.type().name().to_string() + "\'");
      return false;
    }
    case variable_initialization_checker::check_result::
      reference_init_from_temporary_value: {
      raise_error(source, src_range,
                  "Return type is a reference and can not be initialized with "
                  "a temporary value");
      return false;
    }
    default:
      return true;
  }
}

const sema_type*
sema_builder_ast_visitor::try_deduce_currently_parsed_function_return_type()
{
  const auto types_finder = builtin_types_finder{ m_ctx };

  if (m_function_parsing_ctx.return_nodes.empty()) {
    return &types_finder.find_void();
  }

  bool all_same{ true };
  const auto& first_type = m_function_parsing_ctx.return_nodes.front()->type();

  const auto all_expression_types_are_the_same = std::all_of(
    std::cbegin(m_function_parsing_ctx.return_nodes),
    std::cend(m_function_parsing_ctx.return_nodes),
    [&first_type](const auto& node) { return first_type == node->type(); });

  if (all_expression_types_are_the_same) {
    return &first_type;
  }

  raise_error(m_function_parsing_ctx.function_node->name(),
              "incosistent deduction for auto return type");

  const auto source = m_function_parsing_ctx.function_node->name().source();
  for (const auto& ret_node : m_function_parsing_ctx.return_nodes) {
    raise_note(source, ret_node->ast_node().src_range(),
               "deduced as \'" + ret_node->type().name().to_string() + "\'");
  }

  return nullptr;
}

void sema_builder_ast_visitor::visit(
  const ast::variable_declaration_node& node)
{
  const auto& type_representation = node.type();
  const auto should_deduce_type_from_initialization =
    type_representation.is_auto();

  const sema_type* type = nullptr;
  if (!should_deduce_type_from_initialization) {
    type = try_get_or_create_generic_type(m_ctx, type_representation);
    if (!type) {
      // If type has not been found and that's a not an auto type declaration.
      return;
    }

    // If type has been found, check whether it is not void.
    const auto& void_type = builtin_types_finder{ m_ctx }.find_void();
    if (*type == void_type) {
      const auto n = type->name().to_string();
      const auto n2 = void_type.name().to_string();
      raise_error(type_representation.primary_name(),
                  "Variable can not be of void type");
      return;
    }
  }

  if (should_deduce_type_from_initialization &&
      node.initialization() == nullptr) {
    raise_error(
      type_representation.primary_name(),
      "Declaration of a variable with 'auto' type requires an initializer");
    return;
  }

  if (type && type->is_reference() && node.initialization() == nullptr) {
    raise_error(type->name().primary_name(),
                "Declaration of a reference variable requires an initializer");
    return;
  }

  std::unique_ptr<expression_node> initialization;
  if (auto init_node = node.initialization()) {
    initialization = visit_child_expr(*init_node);
    if (!initialization) {
      return;
    }

    if (!should_deduce_type_from_initialization) {
      variable_initialization_checker checker;
      const auto check_result = checker.check(*type, *initialization);

      switch (check_result) {
        case variable_initialization_checker::check_result::can_init: {
          // Can initialize, do nothing.
        } break;
        case variable_initialization_checker::check_result::different_types: {
          raise_error(
            initialization->type().name().primary_name(),
            "Initialization and declared variable type does not match");
          return;
        }
        case variable_initialization_checker::check_result::
          reference_init_from_temporary_value: {
          raise_error(initialization->type().name().primary_name(),
                      "Reference variable can not be initialized with a "
                      "temporary value");
          return;
        }
      }
    }

    if (should_deduce_type_from_initialization) {
      const auto& init_type = initialization->type();
      type =
        &(init_type.is_reference() ? init_type.referenced_type() : init_type);

      // If there was auto with a reference, we need to declare variable with a
      // reference type, e.g. auto& foo = bar();
      if (type_representation.is_reference()) {
        type = m_ctx.find_reference_for(*type);
      }
    }

    initialization =
      convert_to_cast_node_if_need(*type, std::move(initialization));
  }

  m_ids_context.register_identifier(node.name(), *type);
  m_result_node = std::make_unique<variable_declaration_node>(
    node, *type, node.name(), std::move(initialization));
}

void sema_builder_ast_visitor::visit(const ast::for_node& node)
{
  auto guard = ids_guard();

  std::unique_ptr<sema_node> init;
  if (node.init()) {
    init = visit_child(*node.init());
    if (!init) {
      return;
    }
  }

  std::unique_ptr<expression_node> condition;
  if (node.condition()) {
    condition = visit_child_expr(*node.condition());
    if (!condition) {
      return;
    }
  }

  std::unique_ptr<expression_node> iteration;
  if (node.iteration()) {
    iteration = visit_child_expr(*node.iteration());
    if (!iteration) {
      return;
    }
  }

  auto body = visit_child_node<block_node>(node.body());
  if (!body) {
    return;
  }

  m_result_node =
    std::make_unique<for_node>(node, std::move(init), std::move(condition),
                               std::move(iteration), std::move(body));
}

void sema_builder_ast_visitor::visit(const ast::break_node& node)
{
  // Todo: implement
}
}
