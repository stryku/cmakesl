#pragma once

#include "ast/ast_node_visitor.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/qualified_contextes_refs.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_node.hpp"
#include "sema/type_member_info.hpp"

#include <optional>

#include <memory>
#include <vector>

namespace cmsl {
namespace ast {
class ast_node;
class type_representation;
}

namespace errors {
class errors_observer;
enum class error_type;
}

namespace lexer {
class token;
}

namespace sema {
class add_subdirectory_handler;
class block_node;
class builtin_token_provider;
class enum_values_context;
class expression_node;
class factories_provider;
class functions_context;
class identifiers_context;
class import_handler;
class return_node;
class sema_context_impl;
class sema_type;
class types_context;
class user_sema_function;
class variable_declaration_node;
struct function_signature;

struct function_parsing_context
{
  ast::user_function_node* function_node{ nullptr };
  sema_function* function{ nullptr };
  std::vector<const return_node*> return_nodes;

  void reset()
  {
    function_node = nullptr;
    function = nullptr;
    return_nodes.clear();
  }
};

struct parsing_context
{
  function_parsing_context function_parsing_ctx;
  unsigned loop_parsing_counter{ 0u };
};

struct sema_builder_ast_visitor_members
{
  sema_context& generic_types_context;
  sema_context& ctx;
  errors::errors_observer& errors_observer;
  qualified_contextes_refs qualified_ctxs;
  factories_provider& factories;
  const builtin_token_provider& builtin_tokens;
  parsing_context& parsing_ctx;
  builtin_types_accessor builtin_types;
  add_subdirectory_handler& add_subdir_handler;
  import_handler& imports_handler;
};

class sema_builder_ast_visitor : public ast::ast_node_visitor
{
private:
  using param_expressions_t = std::vector<std::unique_ptr<expression_node>>;

public:
  explicit sema_builder_ast_visitor(sema_builder_ast_visitor_members& members);

  void visit(const ast::binary_operator_node& node) override;
  void visit(const ast::block_node& node) override;
  void visit(const ast::bool_value_node& node) override;
  void visit(const ast::break_node& node) override;
  void visit(const ast::class_member_access_node& node) override;
  void visit(const ast::class_node& node) override;
  void visit(const ast::conditional_node& node) override;
  void visit(const ast::designated_initializers_node& node) override;
  void visit(const ast::double_value_node& node) override;
  void visit(const ast::enum_node& node) override;
  void visit(const ast::for_node& node) override;
  void visit(const ast::function_call_node& node) override;
  void visit(const ast::id_node& node) override;
  void visit(const ast::if_else_node& node) override;
  void visit(const ast::import_node& node) override;
  void visit(const ast::initializer_list_node& node) override;
  void visit(const ast::int_value_node& node) override;
  void visit(const ast::member_function_call_node& node) override;
  void visit(const ast::namespace_node& node) override;
  void visit(const ast::return_node& node) override;
  void visit(const ast::standalone_variable_declaration_node& node) override;
  void visit(const ast::string_value_node& node) override;
  void visit(const ast::ternary_operator_node& node) override;
  void visit(const ast::translation_unit_node& node) override;
  void visit(const ast::unary_operator& node) override;
  void visit(const ast::user_function_node& node) override;
  void visit(const ast::variable_declaration_node& node) override;
  void visit(const ast::while_node& node) override;

private:
  function_lookup_result_t find_functions(
    const std::vector<ast::name_with_coloncolon>& names);

  const sema_type* try_get_or_create_generic_type(
    const sema_context& search_context, const ast::type_representation& name);

  const sema_type& create_enum_type(const ast::enum_node& node);

  std::unique_ptr<expression_node> build_function_call(
    const ast::function_call_node& node);
  std::unique_ptr<expression_node> build_add_subdirectory_call(
    const ast::function_call_node& node);

  template <typename T>
  std::unique_ptr<T> to_node(std::unique_ptr<sema_node> node) const;
  std::unique_ptr<expression_node> to_expression(
    std::unique_ptr<sema_node> node) const;

  void raise_note(const lexer::token& token, const std::string& message);
  void raise_note(const source_view& source, const source_range& src_range,
                  const std::string& message);
  void raise_error(const lexer::token& token, const std::string& message);
  void raise_error(const source_view& source, const source_range& src_range,
                   const std::string& message);
  // Returns true if redefined.
  bool raise_error_if_function_redefined(const function_signature& signature);

  void notify_error_observer(const source_view& source,
                             const source_range& src_range,
                             const std::string& message,
                             errors::error_type err_type);

  sema_builder_ast_visitor clone() const;
  sema_builder_ast_visitor clone(sema_context& ctx_to_visit) const;

  template <typename T>
  std::unique_ptr<T> visit_child_node(const ast::ast_node& node);

  template <typename T>
  std::unique_ptr<T> visit_child_node(const ast::ast_node& node,
                                      sema_context& ctx_to_visit);
  std::unique_ptr<sema_node> visit_child(const ast::ast_node& node);
  std::unique_ptr<sema_node> visit_child(const ast::ast_node& node,
                                         sema_context& ctx_to_visit);
  std::unique_ptr<expression_node> visit_child_expr(const ast::ast_node& node);

  std::optional<param_expressions_t> get_function_call_params(
    const std::vector<std::unique_ptr<ast::ast_node>>& passed_params);

  struct function_declaration
  {
    const ast::user_function_node& ast_function_node;
    user_sema_function* fun{ nullptr };
    const ast::block_node& body_to_visit;
    bool should_deduce_return_type{ false };
  };

  struct class_members
  {
    std::vector<member_info> info;
    std::vector<std::unique_ptr<variable_declaration_node>> declarations;
    std::vector<function_declaration> functions;
  };

  std::optional<function_declaration> get_function_declaration_and_add_to_ctx(
    const ast::user_function_node& node, sema_context_impl& ctx);
  std::optional<class_members> collect_class_members_and_add_functions_to_ctx(
    const ast::class_node& node, sema_context_impl& class_context);

  template <unsigned N>
  lexer::token make_token(lexer::token_type token_type, const char (&tok)[N]);

  param_expressions_t convert_params_to_cast_nodes_if_need(
    const function_signature& signature, param_expressions_t params);
  std::unique_ptr<expression_node> convert_to_cast_return_node_if_need(
    std::unique_ptr<expression_node> expression);
  std::unique_ptr<expression_node> convert_to_cast_node_if_need(
    const sema_type& expected_result_type,
    std::unique_ptr<expression_node> expression);

  bool is_last_node_return_node(const block_node& block) const;
  void add_implicit_return_node_if_need(block_node& block);

  bool check_function_return_type(const lexer::token& return_kw,
                                  const expression_node& return_expression);
  const sema_type* try_deduce_currently_parsed_function_return_type();

  bool is_inside_loop() const;

  void add_user_type_default_methods(const sema_type& class_ty,
                                     sema_context& class_ctx);

  bool is_export_allowed() const;

public:
  std::unique_ptr<sema_node> m_result_node;

private:
  sema_builder_ast_visitor_members& m_;
};
}
}
