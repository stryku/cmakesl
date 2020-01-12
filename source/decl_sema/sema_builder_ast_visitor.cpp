#include "decl_sema/sema_builder_ast_visitor.hpp"
#include "common/strings_container.hpp"
#include "decl_sema/sema_nodes.hpp"
#include "lexer/token.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/generic_type_creation_utils.hpp"
#include "sema/sema_type.hpp"
#include "sema/type_builder.hpp"

namespace {
const auto decl_namespace_token =
  cmsl::lexer::make_token(cmsl::lexer::token_type::identifier, "decl");
}

namespace cmsl::decl_sema {
sema_builder_ast_visitor::sema_builder_ast_visitor(
  sema_builder_ast_visitor_members members)
  : m_{ members }
  , m_decl_namespace_ctx_guard{ m_.qualified_ctxs.all_qualified_ctxs_guard(
      decl_namespace_token, /*exported=*/false) }
{
}

sema_builder_ast_visitor::sema_builder_ast_visitor(
  sema_builder_ast_visitor_members members,
  const sema::sema_type* current_component_type)
  : m_{ members }
  , m_current_component_type{ current_component_type }
{
}

sema_builder_ast_visitor::~sema_builder_ast_visitor() = default;

void sema_builder_ast_visitor::visit(
  const decl_ast::component_declaration_node& node)
{
  const auto type_name = node.name();
  const auto found_type =
    m_.qualified_ctxs.types.find_in_current_scope(type_name);
  if (found_type) {
    // Todo: raise type redefinition  error
    return;
  }

  const sema::sema_type* derived_type{ nullptr };
  const component_declaration_node* derived_component_declaration{ nullptr };
  if (const auto derived_type_name = node.derived_type_name()) {
    derived_type =
      m_.qualified_ctxs.types.find_in_current_scope(*derived_type_name);
    if (!derived_type) {
      // Todo: raise derived type not found
      return;
    }

    const auto found_component_declaration_it =
      m_.component_declarations.find(node.name().str());
    derived_component_declaration =
      (found_component_declaration_it == std::cend(m_.component_declarations))
      ? nullptr
      : found_component_declaration_it->second;
  }

  auto& component_context = m_.factories.context_factory().create_class(
    std::string{ type_name.str() }, &m_.ctx);

  auto type_factory = m_.factories.type_factory(m_.qualified_ctxs.types);
  auto& component_type = type_factory.create(
    component_context, ast::type_representation{ type_name }, {},
    /*exported=*/true, /*flags=*/{}, derived_type);
  auto& component_type_ref =
    type_factory.create_reference(component_type, /*exported=*/true);

  m_.ctx.add_type(component_type);
  m_.ctx.add_type(component_type_ref);

  auto types_guard =
    m_.qualified_ctxs.types_ctx_guard(type_name, /*exported=*/true);

  m_current_component_type = &component_type;

  component_node::nodes_t nodes;

  for (const auto& ast_node : node.nodes()) {
    auto child_node = visit_child(*ast_node);
    if (!child_node) {
      return;
    }

    nodes.emplace_back(std::move(child_node));
  }

  auto created_node = std::make_unique<component_declaration_node>(
    node, component_type, derived_component_declaration, node.name(),
    std::move(nodes));

  m_.component_declarations[node.name().str()] = created_node.get();

  m_result_node = std::move(created_node);

  m_current_component_type = nullptr;
}

void sema_builder_ast_visitor::visit(const decl_ast::component_node& node)
{
  const auto type_name = node.name();
  const auto found_type =
    m_.qualified_ctxs.types.find_in_current_scope(type_name);
  if (!found_type) {
    // Todo: raise type not found error
    return;
  }

  const auto found_component_declaration_it =
    m_.component_declarations.find(node.name().str());
  const auto component_declaration =
    (found_component_declaration_it == std::cend(m_.component_declarations))
    ? nullptr
    : found_component_declaration_it->second;

  m_current_component_type = found_type;

  component_node::nodes_t nodes;

  for (const auto& ast_node : node.nodes()) {
    auto child_node = visit_child(*ast_node);
    if (!child_node) {
      return;
    }

    nodes.emplace_back(std::move(child_node));
  }

  m_result_node = std::make_unique<component_node>(
    node, *found_type, component_declaration, node.name(), std::move(nodes));

  m_current_component_type = nullptr;
}

void sema_builder_ast_visitor::visit(const decl_ast::property_node& node)
{
  auto property_access =
    visit_child_type<property_access_node>(node.property_access());
  if (!property_access) {
    return;
  }

  auto value = visit_child_expr(node.value());
  if (!value) {
    return;
  }

  if (property_access->type().decayed() != value->type().decayed()) {
    // Todo: raise different property type and init value
    return;
  }

  m_result_node = std::make_unique<property_node>(
    node, std::move(property_access), std::move(value));
}

void sema_builder_ast_visitor::visit(
  const decl_ast::property_access_node& node)
{
  std::vector<sema::member_info> property_access;

  const auto* current_type = m_current_component_type;

  for (const auto& name : node.property_access()) {
    const auto found_property = current_type->find_member(name.str());
    if (!found_property) {
      // Todo: raise property not found type
      return;
    }

    property_access.emplace_back(*found_property);
    current_type = &found_property->ty;
  }

  m_result_node =
    std::make_unique<property_access_node>(node, std::move(property_access));
}

void sema_builder_ast_visitor::visit(const decl_ast::bool_value_node& node)
{
  const auto value = (node.token().str() == "true");
  m_result_node =
    std::make_unique<bool_value_node>(node, value, m_.builtin_types.bool_);
}

void sema_builder_ast_visitor::visit(const decl_ast::int_value_node& node)
{
  char* end;
  const auto value =
    std::strtol(node.token().str().data(), &end, 10); // todo: handle hex etc
  m_result_node =
    std::make_unique<int_value_node>(node, value, m_.builtin_types.int_);
}

void sema_builder_ast_visitor::visit(const decl_ast::double_value_node& node)
{
  char* end;
  const auto value =
    std::strtod(node.token().str().data(), &end); // todo: handle hex etc
  m_result_node =
    std::make_unique<double_value_node>(node, value, m_.builtin_types.double_);
}

void sema_builder_ast_visitor::visit(const decl_ast::string_value_node& node)
{
  // At this point node contains string value including "". We need to get rid
  // of them.
  const auto node_string = node.token().str();
  const auto string_without_quotation_marks =
    cmsl::string_view{ std::next(node_string.data()),
                       node_string.size() - 2u };

  m_result_node = std::make_unique<string_value_node>(
    node, string_without_quotation_marks, m_.builtin_types.string);
}

void sema_builder_ast_visitor::visit(const decl_ast::list_node& node)
{
  list_node::values_t values;

  for (const auto& ast_value : node.nodes()) {
    auto value_expression = visit_child_expr(*ast_value);
    if (!value_expression) {
      return;
    }

    values.emplace_back(std::move(value_expression));
  }

  m_result_node = std::make_unique<list_node>(node, std::move(values),
                                              list_of_strings_type());
}

sema_builder_ast_visitor sema_builder_ast_visitor::clone()
{
  return sema_builder_ast_visitor{ m_, m_current_component_type };
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

std::unique_ptr<expression_node> sema_builder_ast_visitor::visit_child_expr(
  const decl_ast::ast_node& node)
{
  return to_node<expression_node>(visit_child(node));
}

std::unique_ptr<sema_node> sema_builder_ast_visitor::visit_child(
  const decl_ast::ast_node& node)
{
  auto v = clone();
  node.visit(v);
  return std::move(v.m_result_node);
}

const sema::sema_type& sema_builder_ast_visitor::list_of_strings_type()
{
  auto creation_utils = sema::generic_type_creation_utils{
    m_.generic_types_context, m_.factories,     m_.errs,
    m_.builtin_tokens,        m_.builtin_types, m_.qualified_ctxs.types
  };

  return creation_utils.list_of_strings();
}

template <typename T>
std::unique_ptr<T> sema_builder_ast_visitor::visit_child_type(
  const decl_ast::ast_node& node)
{
  return to_node<T>(visit_child(node));
}

void sema_builder_ast_visitor::visit(
  const decl_ast::cmake_variable_access_node& node)
{
  const auto& type =
    [this, as_type = node.as_type().str()]() -> const sema::sema_type& {
    if (as_type == "as_bool") {
      return m_.builtin_types.bool_;
    }

    if (as_type == "as_int") {
      return m_.builtin_types.int_;
    }

    if (as_type == "as_double") {
      return m_.builtin_types.double_;
    }

    if (as_type == "as_string") {
      return m_.builtin_types.string;
    }

    if (as_type == "as_list") {
      return list_of_strings_type();
    }

    CMSL_UNREACHABLE("Unknown as type");
  }();

  m_result_node = std::make_unique<cmake_variable_access_node>(
    node, node.variable_name(), type);
}

void sema_builder_ast_visitor::visit(
  const decl_ast::translation_unit_node& node)
{
  translation_unit_node::nodes_t nodes;

  for (const auto& ast_node : node.nodes()) {
    auto child = visit_child(*ast_node);
    if (!child) {
      return;
    }

    nodes.emplace_back(std::move(child));
  }

  m_result_node =
    std::make_unique<translation_unit_node>(node, std::move(nodes));
}
}
