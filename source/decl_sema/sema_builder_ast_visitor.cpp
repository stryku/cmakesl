#include "decl_sema/sema_builder_ast_visitor.hpp"
#include "common/strings_container.hpp"
#include "decl_sema/sema_nodes.hpp"

namespace cmsl::decl_sema {

sema_builder_ast_visitor::sema_builder_ast_visitor(strings_container& strings)
  : m_strings_container{ strings }
{
}

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

  m_result_node = std::make_unique<component_node>(node, std::move(nodes));
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
  m_result_node = std::make_unique<bool_value_node>(node, value);
}

void sema_builder_ast_visitor::visit(const decl_ast::int_value_node& node)
{
  char* end;
  const auto value =
    std::strtol(node.token().str().data(), &end, 10); // todo: handle hex etc
  m_result_node = std::make_unique<int_value_node>(node, value);
}

void sema_builder_ast_visitor::visit(const decl_ast::double_value_node& node)
{
  char* end;
  const auto value =
    std::strtod(node.token().str().data(), &end); // todo: handle hex etc
  m_result_node = std::make_unique<double_value_node>(node, value);
}

void sema_builder_ast_visitor::visit(const decl_ast::string_value_node& node)
{
  const auto stored_view =
    m_strings_container.store(std::string{ node.token().str() });
  m_result_node = std::make_unique<string_value_node>(node, stored_view);
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

  m_result_node = std::make_unique<list_node>(node, std::move(values));
}

sema_builder_ast_visitor sema_builder_ast_visitor::clone()
{
  return sema_builder_ast_visitor{ m_strings_container };
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
}
