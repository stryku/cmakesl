#include "decl_sema/sema_builder_ast_visitor.hpp"
#include "common/strings_container.hpp"
#include "decl_sema/sema_nodes.hpp"
#include "sema/generic_type_creation_utils.hpp"

namespace cmsl::decl_sema {

sema_builder_ast_visitor::sema_builder_ast_visitor(
  sema_builder_ast_visitor_members members)
  : m_{ members }
{
}

sema_builder_ast_visitor::~sema_builder_ast_visitor() = default;

void sema_builder_ast_visitor::visit(const decl_ast::component_node& node)
{
  component_node::nodes_t nodes;

  for (const auto& ast_node : node.nodes()) {
    auto child_node = visit_child(*ast_node);
    if (!child_node) {
      return;
    }

    nodes.emplace_back(std::move(child_node));
  }

  m_result_node =
    std::make_unique<component_node>(node, node.name(), std::move(nodes));
}

void sema_builder_ast_visitor::visit(const decl_ast::property_node& node)
{
  auto value = visit_child_expr(node.value());
  if (!value) {
    return;
  }

  m_result_node =
    std::make_unique<property_node>(node, node.name(), std::move(value));
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
  return sema_builder_ast_visitor{ m_ };
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
}
