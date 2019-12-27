#pragma once

#include "decl_ast/ast_node_visitor.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/qualified_contextes_refs.hpp"

#include <memory>

namespace cmsl {

class strings_container;

namespace decl_ast {
class ast_node;
}

namespace errors {
class errors_observer;
}

namespace sema {
class builtin_token_provider;
class factories_provider;
class sema_context;
class types_context;
}

namespace decl_sema {
class sema_node;
class expression_node;
class property_access_node;

struct sema_builder_ast_visitor_members
{
  errors::errors_observer& errs;
  sema::sema_context& generic_types_context;
  sema::sema_context& ctx;
  sema::factories_provider& factories;
  sema::builtin_token_provider& builtin_tokens;
  sema::builtin_types_accessor builtin_types;
  sema::qualified_contextes_refs qualified_ctxs;
  strings_container& strings;
};

class sema_builder_ast_visitor : public decl_ast::ast_node_visitor
{
public:
  explicit sema_builder_ast_visitor(sema_builder_ast_visitor_members members);
  ~sema_builder_ast_visitor() override;

  void visit(const decl_ast::bool_value_node& node) override;
  void visit(const decl_ast::component_node& node) override;
  void visit(const decl_ast::double_value_node& node) override;
  void visit(const decl_ast::int_value_node& node) override;
  void visit(const decl_ast::list_node& node) override;
  void visit(const decl_ast::property_access_node& node) override;
  void visit(const decl_ast::property_node& node) override;
  void visit(const decl_ast::string_value_node& node) override;

private:
  explicit sema_builder_ast_visitor(
    sema_builder_ast_visitor_members members,
    const sema::sema_type* current_component_type);

  template <typename T>
  std::unique_ptr<T> visit_child_type(const decl_ast::ast_node& node);
  std::unique_ptr<sema_node> visit_child(const decl_ast::ast_node& node);
  std::unique_ptr<expression_node> visit_child_expr(
    const decl_ast::ast_node& node);

  sema_builder_ast_visitor clone();

  template <typename T>
  std::unique_ptr<T> to_node(std::unique_ptr<sema_node> node) const;
  std::unique_ptr<expression_node> to_expression(
    std::unique_ptr<sema_node> node) const;

  const sema::sema_type& list_of_strings_type();

  std::unique_ptr<property_access_node> validate_and_create_property_access(
    const sema::sema_type& component_type,
    const std::vector<lexer::token>& property_names);

public:
  std::unique_ptr<sema_node> m_result_node;

private:
  sema_builder_ast_visitor_members m_;
  std::optional<sema::qualified_contextes_refs::all_qualified_contextes_guard>
    m_decl_namespace_ctx_guard;
  const sema::sema_type* m_current_component_type{ nullptr };
};
}
}
