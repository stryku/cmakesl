#pragma once

#include "decl_ast/ast_node_visitor.hpp"

#include <memory>

namespace cmsl {

class strings_container;

namespace decl_ast {
class ast_node;
}

namespace decl_sema {
class sema_node;
class expression_node;

class sema_builder_ast_visitor : public decl_ast::ast_node_visitor
{
public:
  explicit sema_builder_ast_visitor(strings_container& strings);
  ~sema_builder_ast_visitor();

  void visit(const decl_ast::component_node& node) override;
  void visit(const decl_ast::property_node& node) override;
  void visit(const decl_ast::bool_value_node& node) override;
  void visit(const decl_ast::int_value_node& node) override;
  void visit(const decl_ast::double_value_node& node) override;
  void visit(const decl_ast::string_value_node& node) override;
  void visit(const decl_ast::list_node& node) override;

private:
  std::unique_ptr<sema_node> visit_child(const decl_ast::ast_node& node);
  std::unique_ptr<expression_node> visit_child_expr(
    const decl_ast::ast_node& node);

  sema_builder_ast_visitor clone();

  template <typename T>
  std::unique_ptr<T> to_node(std::unique_ptr<sema_node> node) const;
  std::unique_ptr<expression_node> to_expression(
    std::unique_ptr<sema_node> node) const;

public:
  std::unique_ptr<sema_node> m_result_node;

private:
  strings_container& m_strings_container;
};
}
}