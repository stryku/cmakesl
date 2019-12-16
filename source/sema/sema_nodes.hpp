#pragma once

#include "ast/ast_node.hpp"
#include "ast/namespace_node.hpp"
#include "ast/qualified_name.hpp"
#include "common/int_alias.hpp"
#include "lexer/token.hpp"
#include "sema/sema_function.hpp"
#include "sema/sema_node.hpp"
#include "sema/sema_node_visitor.hpp"
#include "sema/sema_type.hpp"

#include <memory>

#define VISIT_METHOD                                                          \
  void visit(sema_node_visitor& visitor) const override                       \
  {                                                                           \
    visitor.visit(*this);                                                     \
  }

using token_t = cmsl::lexer::token;

namespace cmsl::sema {
class expression_node : public sema_node
{
public:
  using sema_node::sema_node;

  virtual const sema_type& type() const = 0;
  virtual bool produces_temporary_value() const = 0;
};

template <typename T>
class value_node : public expression_node
{
public:
  explicit value_node(const ast::ast_node& ast_node, const sema_type& t, T val)
    : expression_node{ ast_node }
    , m_type{ t }
    , m_value{ val }
  {
  }

  T value() const { return m_value; }

  const sema_type& type() const override { return m_type; }

  bool produces_temporary_value() const override { return true; }

private:
  const sema_type& m_type;
  T m_value;
};

class bool_value_node : public value_node<bool>
{
public:
  explicit bool_value_node(const ast::ast_node& ast_node, const sema_type& t,
                           bool val)
    : value_node{ ast_node, t, val }
  {
  }

  VISIT_METHOD
};

class int_value_node : public value_node<int_t>
{
public:
  explicit int_value_node(const ast::ast_node& ast_node, const sema_type& t,
                          int_t val)
    : value_node{ ast_node, t, val }
  {
  }

  VISIT_METHOD
};

class double_value_node : public value_node<double>
{
public:
  explicit double_value_node(const ast::ast_node& ast_node, const sema_type& t,
                             double val)
    : value_node{ ast_node, t, val }
  {
  }

  VISIT_METHOD
};

class string_value_node : public value_node<cmsl::string_view>
{
public:
  explicit string_value_node(const ast::ast_node& ast_node, const sema_type& t,
                             cmsl::string_view val)
    : value_node{ ast_node, t, val }
  {
  }

  VISIT_METHOD
};

class id_node : public expression_node
{
public:
  explicit id_node(const ast::ast_node& ast_node, const sema_type& t,
                   std::vector<ast::name_with_coloncolon> names,
                   unsigned index)
    : expression_node{ ast_node }
    , m_type{ t }
    , m_names{ std::move(names) }
    , m_index{ index }
  {
  }

  const sema_type& type() const override { return m_type; }

  bool produces_temporary_value() const override { return false; }

  const std::vector<ast::name_with_coloncolon>& names() const
  {
    return m_names;
  }

  unsigned index() const { return m_index; }

  VISIT_METHOD

private:
  const sema_type& m_type;
  std::vector<ast::name_with_coloncolon> m_names;
  const unsigned m_index;
};

class enum_constant_access_node : public expression_node
{
public:
  explicit enum_constant_access_node(
    const ast::ast_node& ast_node, const sema_type& t,
    std::vector<ast::name_with_coloncolon> names, unsigned enum_value,
    unsigned index)
    : expression_node{ ast_node }
    , m_type{ t }
    , m_names{ std::move(names) }
    , m_value{ enum_value }
    , m_index{ index }
  {
  }

  const sema_type& type() const override { return m_type; }

  bool produces_temporary_value() const override { return false; }

  const std::vector<ast::name_with_coloncolon>& names() const
  {
    return m_names;
  }

  unsigned value() const { return m_value; }
  unsigned index() const { return m_index; }

  VISIT_METHOD

private:
  const sema_type& m_type;
  std::vector<ast::name_with_coloncolon> m_names;
  const unsigned m_value;
  const unsigned m_index;
};

// Todo: handle return without expression when implementing void type.
class return_node : public expression_node
{
public:
  explicit return_node(const ast::ast_node& ast_node,
                       std::unique_ptr<expression_node> expr)
    : expression_node{ ast_node }
    , m_expr{ std::move(expr) }
  {
    m_expr->set_parent(*this, passkey{});
  }

  const sema_type& type() const override { return m_expr->type(); }

  bool produces_temporary_value() const override
  {
    return !type().is_reference();
  }

  const expression_node& expression() const { return *m_expr; }

  VISIT_METHOD

private:
  std::unique_ptr<expression_node> m_expr;
};

class binary_operator_node : public expression_node
{
public:
  explicit binary_operator_node(const ast::ast_node& ast_node,
                                std::unique_ptr<expression_node> lhs,
                                lexer::token op,
                                const sema_function& operator_function,
                                std::unique_ptr<expression_node> rhs,
                                const sema_type& result_type)
    : expression_node{ ast_node }
    , m_lhs{ std::move(lhs) }
    , m_operator{ op }
    , m_operator_function{ operator_function }
    , m_rhs{ std::move(rhs) }
    , m_type{ result_type }
  {
    m_lhs->set_parent(*this, passkey{});
    m_rhs->set_parent(*this, passkey{});
  }

  const expression_node& lhs() const { return *m_lhs; }

  lexer::token op() const { return m_operator; }

  const sema_function& operator_function() const
  {
    return m_operator_function;
  }

  const expression_node& rhs() const { return *m_rhs; }

  const sema_type& type() const override { return m_type; }

  bool produces_temporary_value() const override
  {
    return !type().is_reference();
  }

  VISIT_METHOD

private:
  std::unique_ptr<expression_node> m_lhs;
  lexer::token m_operator; // Todo: introduce an operator struct that holds
                           // token and operator type
  const sema_function& m_operator_function;
  std::unique_ptr<expression_node> m_rhs;
  const sema_type& m_type;
};

class variable_declaration_node : public sema_node
{
public:
  explicit variable_declaration_node(
    const ast::ast_node& ast_node, const sema_type& type, lexer::token name,
    std::unique_ptr<expression_node> initialization, unsigned index)
    : sema_node{ ast_node }
    , m_index{ index }
    , m_type{ type }
    , m_name{ name }
    , m_initialization{ std::move(initialization) }
  {
    if (initialization) {
      initialization->set_parent(*this, passkey{});
    }
  }

  const sema_type& type() const { return m_type; }

  lexer::token name() const { return m_name; }

  const sema_node* initialization() const { return m_initialization.get(); }
  unsigned index() const { return m_index; }

  VISIT_METHOD

private:
  unsigned m_index;
  const sema_type& m_type;
  const lexer::token m_name;
  std::unique_ptr<expression_node> m_initialization;
};

class call_node : public expression_node
{
public:
  using param_expressions_t = std::vector<std::unique_ptr<expression_node>>;

  explicit call_node(const ast::ast_node& ast_node,
                     const sema_function& function, param_expressions_t params,
                     const token_t& call_name)
    : expression_node{ ast_node }
    , m_function{ function }
    , m_params{ std::move(params) }
    , m_call_name{ call_name }
  {
    for (auto& param : m_params) {
      param->set_parent(*this, passkey{});
    }
  }

  const sema_type& type() const override { return m_function.return_type(); }

  const param_expressions_t& param_expressions() const { return m_params; }

  const lexer::token& name() const { return m_function.signature().name; }

  const lexer::token& call_name() const { return m_call_name; }

  const sema_function& function() const { return m_function; }

  bool produces_temporary_value() const override
  {
    return !type().is_reference();
  }

private:
  const sema_function& m_function;
  param_expressions_t m_params;
  token_t m_call_name;
};

class function_call_node : public call_node
{
public:
  explicit function_call_node(const ast::ast_node& ast_node,
                              const sema_function& function,
                              param_expressions_t params,
                              const token_t& call_name)
    : call_node{ ast_node, function, std::move(params), call_name }
  {
  }

  VISIT_METHOD
};

class member_function_call_node : public call_node
{
public:
  explicit member_function_call_node(const ast::ast_node& ast_node,
                                     std::unique_ptr<expression_node> lhs,
                                     const sema_function& function,
                                     param_expressions_t params,
                                     const token_t& call_name)
    : call_node{ ast_node, function, std::move(params), call_name }
    , m_lhs{ std::move(lhs) }
  {
    m_lhs->set_parent(*this, passkey{});
  }

  const expression_node& lhs() const { return *m_lhs; }

  VISIT_METHOD

private:
  std::unique_ptr<expression_node> m_lhs;
};

class implicit_member_function_call_node : public call_node
{
public:
  explicit implicit_member_function_call_node(const ast::ast_node& ast_node,
                                              const sema_function& function,
                                              param_expressions_t params,
                                              const token_t& call_name)
    : call_node{ ast_node, function, std::move(params), call_name }
  {
  }

  VISIT_METHOD
};

class constructor_call_node : public call_node
{
public:
  explicit constructor_call_node(const ast::ast_node& ast_node,
                                 const sema_type& class_type,
                                 const sema_function& function,
                                 param_expressions_t params,
                                 const token_t& call_name)
    : call_node{ ast_node, function, std::move(params), call_name }
    , m_class_type{ class_type }
  {
  }

  const sema_type& type() const override { return m_class_type; }

  VISIT_METHOD

private:
  const sema_type& m_class_type;
};

class add_subdirectory_node : public call_node
{
public:
  explicit add_subdirectory_node(
    const ast::ast_node& ast_node,
    std::unique_ptr<string_value_node> directory_name,
    const sema_function& function, param_expressions_t params,
    const token_t& call_name)
    : call_node{ ast_node, function, std::move(params), call_name }
    , m_directory_name{ std::move(directory_name) }
  {
    m_directory_name->set_parent(*this, passkey{});
  }

  const string_value_node& dir_name() const { return *m_directory_name; }

  VISIT_METHOD

private:
  std::unique_ptr<string_value_node> m_directory_name;
};

class add_subdirectory_with_declarative_script_node : public expression_node
{
public:
  explicit add_subdirectory_with_declarative_script_node(
    const ast::ast_node& ast_node,
    std::unique_ptr<string_value_node> directory_name,
    const sema_function& component_creation_function)
    : expression_node{ ast_node }
    , m_directory_name{ std::move(directory_name) }
    , m_component_creation_function{ component_creation_function }
  {
    m_directory_name->set_parent(*this, passkey{});
  }

  const string_value_node& dir_name() const { return *m_directory_name; }

  const sema_type& type() const override
  {
    return m_component_creation_function.return_type();
  }

  const sema_function& component_creation_function() const
  {
    return m_component_creation_function;
  }

  bool produces_temporary_value() const override { return true; }

  VISIT_METHOD

private:
  std::unique_ptr<string_value_node> m_directory_name;
  const sema_function& m_component_creation_function;
};

class add_subdirectory_with_old_script_node : public expression_node
{
public:
  explicit add_subdirectory_with_old_script_node(
    const ast::ast_node& ast_node,
    std::unique_ptr<string_value_node> directory_name,
    const sema_type& void_type)
    : expression_node{ ast_node }
    , m_directory_name{ std::move(directory_name) }
    , m_void_type{ void_type }
  {
    m_directory_name->set_parent(*this, passkey{});
  }

  const string_value_node& dir_name() const { return *m_directory_name; }

  const sema_type& type() const override { return m_void_type; }

  bool produces_temporary_value() const override { return true; }

  VISIT_METHOD

private:
  std::unique_ptr<string_value_node> m_directory_name;
  const sema_type& m_void_type;
};

class add_declarative_file_node : public call_node
{
public:
  explicit add_declarative_file_node(
    const ast::ast_node& ast_node,
    std::unique_ptr<string_value_node> file_path,
    const sema_function& function, const token_t& call_name)
    : call_node{ ast_node, function, {}, call_name }
    , m_file_path{ std::move(file_path) }
  {
    m_file_path->set_parent(*this, passkey{});
  }

  const string_value_node& file_path() const { return *m_file_path; }

  VISIT_METHOD

private:
  std::unique_ptr<string_value_node> m_file_path;
};

class block_node_manipulator;

class block_node : public sema_node
{
private:
  using nodes_t = std::vector<std::unique_ptr<sema_node>>;

public:
  explicit block_node(const ast::ast_node& ast_node, nodes_t nodes)
    : sema_node{ ast_node }
    , m_nodes{ std::move(nodes) }
  {
    for (auto& node : m_nodes) {
      node->set_parent(*this, passkey{});
    }
  }

  const nodes_t& nodes() const { return m_nodes; }

  VISIT_METHOD

private:
  friend class block_node_manipulator;

  nodes_t m_nodes;
};

class function_node : public sema_node
{
public:
  explicit function_node(const ast::ast_node& ast_node,
                         const sema_function& function,
                         std::unique_ptr<block_node> body)
    : sema_node{ ast_node }
    , m_function{ function }
    , m_body{ std::move(body) }
  {
    m_body->set_parent(*this, passkey{});
  }

  const function_signature& signature() const
  {
    return m_function.signature();
  }

  const sema_function& function() const { return m_function; }

  const sema_type& return_type() const { return m_function.return_type(); }

  const block_node& body() const { return *m_body; }

  VISIT_METHOD

private:
  const sema_function& m_function;
  std::unique_ptr<block_node> m_body;
};

class class_node : public sema_node
{
private:
  using members_t = std::vector<std::unique_ptr<variable_declaration_node>>;
  using functions_t = std::vector<std::unique_ptr<function_node>>;

public:
  explicit class_node(const ast::ast_node& ast_node, lexer::token name,
                      members_t members, functions_t functions)
    : sema_node{ ast_node }
    , m_name{ name }
    , m_members{ std::move(members) }
    , m_functions{ std::move(functions) }
  {
    for (auto& member : m_members) {
      member->set_parent(*this, passkey{});
    }
    for (auto& function : m_functions) {
      function->set_parent(*this, passkey{});
    }
  }

  lexer::token name() const { return m_name; }

  const members_t& members() const { return m_members; }

  const functions_t& functions() const { return m_functions; }

  VISIT_METHOD

private:
  lexer::token m_name;
  members_t m_members;
  functions_t m_functions;
};

class conditional_node : public sema_node
{
public:
  explicit conditional_node(const ast::ast_node& ast_node,
                            std::unique_ptr<expression_node> condition,
                            std::unique_ptr<block_node> body)
    : sema_node{ ast_node }
    , m_condition{ std::move(condition) }
    , m_body{ std::move(body) }
  {
    m_condition->set_parent(*this, passkey{});
    m_body->set_parent(*this, passkey{});
  }

  const expression_node& get_condition() const { return *m_condition; }

  const block_node& get_body() const { return *m_body; }

  VISIT_METHOD

private:
  std::unique_ptr<expression_node> m_condition;
  std::unique_ptr<block_node> m_body;
};

class while_node : public sema_node
{
public:
  explicit while_node(const ast::ast_node& ast_node,
                      std::unique_ptr<conditional_node> condition)
    : sema_node{ ast_node }
    , m_conditional{ std::move(condition) }
  {
    m_conditional->set_parent(*this, passkey{});
  }

  const expression_node& condition() const
  {
    return m_conditional->get_condition();
  }
  const block_node& body() const { return m_conditional->get_body(); }

  VISIT_METHOD

private:
  std::unique_ptr<conditional_node> m_conditional;
};

class if_else_node : public sema_node
{
public:
  using ifs_t = std::vector<std::unique_ptr<conditional_node>>;

  explicit if_else_node(const ast::ast_node& ast_node, ifs_t ifs,
                        std::unique_ptr<block_node> else_node)
    : sema_node{ ast_node }
    , m_ifs{ std::move(ifs) }
    , m_else{ std::move(else_node) }
  {
    for (auto& if_ : m_ifs) {
      if_->set_parent(*this, passkey{});
    }

    if (m_else) {
      m_else->set_parent(*this, passkey{});
    }
  }

  const ifs_t& ifs() const { return m_ifs; }

  const block_node* else_body() const { return m_else.get(); }

  VISIT_METHOD

private:
  ifs_t m_ifs;
  std::unique_ptr<block_node> m_else;
};

class class_member_access_node : public expression_node
{
public:
  explicit class_member_access_node(const ast::ast_node& ast_node,
                                    std::unique_ptr<expression_node> lhs,
                                    const token_t& member_access_name,
                                    member_info member_info)
    : expression_node{ ast_node }
    , m_lhs{ std::move(lhs) }
    , m_member_access_name{ member_access_name }
    , m_member_info{ member_info }
  {
    m_lhs->set_parent(*this, passkey{});
  }

  const expression_node& lhs() const { return *m_lhs; }

  const sema_type& type() const override { return m_member_info.ty; }

  bool produces_temporary_value() const override { return false; }

  const lexer::token& member_name() const { return m_member_info.name; }
  const lexer::token& member_access_name() const
  {
    return m_member_access_name;
  }
  unsigned member_index() const { return m_member_info.index; }

  VISIT_METHOD

private:
  std::unique_ptr<expression_node> m_lhs;
  token_t m_member_access_name;
  member_info m_member_info;
};

class translation_unit_node : public sema_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<sema_node>>;

  explicit translation_unit_node(const ast::ast_node& ast_node,
                                 const sema_context& ctx, nodes_t nodes)
    : sema_node{ ast_node }
    , m_ctx{ ctx }
    , m_nodes{ std::move(nodes) }
  {
    for (auto& node : m_nodes) {
      node->set_parent(*this, passkey{});
    }
  }

  const nodes_t& nodes() const { return m_nodes; }

  const sema_context& context() const { return m_ctx; }

  VISIT_METHOD

private:
  const sema_context& m_ctx;
  nodes_t m_nodes;
};

class cast_to_reference_node : public expression_node
{
public:
  explicit cast_to_reference_node(const ast::ast_node& ast_node,
                                  const sema_type& t,
                                  std::unique_ptr<expression_node> expr)
    : expression_node{ ast_node }
    , m_type{ t }
    , m_expr{ std::move(expr) }
  {
    m_expr->set_parent(*this, passkey{});
  }

  const sema_type& type() const override { return m_type; }

  bool produces_temporary_value() const override { return false; }

  const expression_node& expression() const { return *m_expr; }

  VISIT_METHOD

private:
  const sema_type& m_type;
  std::unique_ptr<expression_node> m_expr;
};

class cast_to_value_node : public expression_node
{
public:
  explicit cast_to_value_node(const ast::ast_node& ast_node,
                              const sema_type& t,
                              std::unique_ptr<expression_node> expr)
    : expression_node{ ast_node }
    , m_type{ t }
    , m_expr{ std::move(expr) }
  {
    m_expr->set_parent(*this, passkey{});
  }

  const sema_type& type() const override { return m_type; }

  bool produces_temporary_value() const override { return false; }

  const expression_node& expression() const { return *m_expr; }

  VISIT_METHOD

private:
  const sema_type& m_type;
  std::unique_ptr<expression_node> m_expr;
};

class initializer_list_node : public expression_node
{
public:
  explicit initializer_list_node(
    const ast::ast_node& ast_node, const sema_type& t,
    std::vector<std::unique_ptr<expression_node>> values)
    : expression_node{ ast_node }
    , m_type{ t }
    , m_values{ std::move(values) }
  {
    for (auto& value : m_values) {
      value->set_parent(*this, passkey{});
    }
  }

  const sema_type& type() const override { return m_type; }

  bool produces_temporary_value() const override { return true; }

  const std::vector<std::unique_ptr<expression_node>>& values() const
  {
    return m_values;
  }

  VISIT_METHOD

private:
  const sema_type& m_type;
  std::vector<std::unique_ptr<expression_node>> m_values;
};

class implicit_return_node : public expression_node
{
public:
  explicit implicit_return_node(const ast::ast_node& ast_node,
                                const sema_type& type)
    : expression_node{ ast_node }
    , m_type{ type }
  {
  }

  const sema_type& type() const override { return m_type; }
  virtual bool produces_temporary_value() const override { return true; }

  source_location begin_location() const override
  {
    return ast_node().end_location();
  }
  source_location end_location() const override
  {
    return ast_node().end_location();
  }

  VISIT_METHOD

private:
  const sema_type& m_type;
};

class for_node : public sema_node
{
public:
  explicit for_node(const ast::ast_node& ast_node,
                    std::unique_ptr<sema_node> init,
                    std::unique_ptr<expression_node> condition,
                    std::unique_ptr<expression_node> iteration,
                    std::unique_ptr<block_node> body)
    : sema_node{ ast_node }
    , m_init{ std::move(init) }
    , m_condition{ std::move(condition) }
    , m_iteration{ std::move(iteration) }
    , m_body{ std::move(body) }
  {
    if (m_init) {
      m_init->set_parent(*this, passkey{});
    }
    if (m_condition) {
      m_condition->set_parent(*this, passkey{});
    }
    if (m_iteration) {
      m_iteration->set_parent(*this, passkey{});
    }
    m_body->set_parent(*this, passkey{});
  }

  const sema_node* init() const { return m_init.get(); }
  const expression_node* condition() const { return m_condition.get(); }
  const expression_node* iteration() const { return m_iteration.get(); }
  const block_node& body() const { return *m_body; }

  VISIT_METHOD

private:
  std::unique_ptr<sema_node> m_init;
  std::unique_ptr<expression_node> m_condition;
  std::unique_ptr<expression_node> m_iteration;
  std::unique_ptr<block_node> m_body;
};

class break_node : public sema_node
{
public:
  explicit break_node(const ast::ast_node& ast_node)
    : sema_node{ ast_node }
  {
  }

  VISIT_METHOD
};

class namespace_node : public sema_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<sema_node>>;
  using names_t = std::vector<ast::name_with_coloncolon>;

  explicit namespace_node(const ast::ast_node& ast_node, names_t names,
                          nodes_t nodes)
    : sema_node{ ast_node }
    , m_nodes{ std::move(nodes) }
    , m_names{ std::move(names) }
  {
    for (auto& node : m_nodes) {
      node->set_parent(*this, passkey{});
    }
  }

  VISIT_METHOD

  const nodes_t& nodes() const { return m_nodes; }

  const names_t& names() const { return m_names; }

private:
  nodes_t m_nodes;
  names_t m_names;
};

class ternary_operator_node : public expression_node
{
public:
  explicit ternary_operator_node(const ast::ast_node& ast_node,
                                 std::unique_ptr<expression_node> condition,
                                 std::unique_ptr<expression_node> true_,
                                 std::unique_ptr<expression_node> false_)
    : expression_node{ ast_node }
    , m_condition{ std::move(condition) }
    , m_true{ std::move(true_) }
    , m_false{ std::move(false_) }
  {
    m_condition->set_parent(*this, passkey{});
    m_true->set_parent(*this, passkey{});
    m_false->set_parent(*this, passkey{});
  }

  const expression_node& condition() const { return *m_condition; }
  const expression_node& true_() const { return *m_true; }
  const expression_node& false_() const { return *m_false; }

  const sema_type& type() const override { return m_true->type(); }

  bool produces_temporary_value() const override
  {
    return m_true->produces_temporary_value() ||
      m_false->produces_temporary_value();
  }

  VISIT_METHOD

private:
  std::unique_ptr<expression_node> m_condition;
  std::unique_ptr<expression_node> m_true;
  std::unique_ptr<expression_node> m_false;
};

class designated_initializers_node : public expression_node
{
public:
  struct initializer
  {
    token_t name;
    std::unique_ptr<expression_node> init;
  };

  using initializers_t = std::vector<initializer>;

  explicit designated_initializers_node(const ast::ast_node& ast_node,
                                        const sema_type& ty,
                                        initializers_t initializers)
    : expression_node{ ast_node }
    , m_type{ ty }
    , m_initializers{ std::move(initializers) }
  {
    for (auto& init : m_initializers) {
      if (init.init) {
        init.init->set_parent(*this, passkey{});
      }
    }
  }

  const initializers_t& initializers() const { return m_initializers; }

  const sema_type& type() const override { return m_type; }

  bool produces_temporary_value() const override { return true; }

  VISIT_METHOD

private:
  const sema_type& m_type;
  initializers_t m_initializers;
};

class unary_operator_node : public expression_node
{
public:
  explicit unary_operator_node(const ast::ast_node& ast_node, token_t op,
                               std::unique_ptr<expression_node> expression,
                               const sema_function& function)
    : expression_node{ ast_node }
    , m_expression{ std::move(expression) }
    , m_function{ function }
  {
    m_expression->set_parent(*this, passkey{});
  }

  const token_t& operator_() const { return m_operator; }

  const expression_node& expression() const { return *m_expression; }

  const sema_function& function() const { return m_function; }

  const sema_type& type() const override { return m_function.return_type(); }

  bool produces_temporary_value() const override
  {
    return !m_function.return_type().is_reference();
  }

  VISIT_METHOD

private:
  token_t m_operator;
  std::unique_ptr<expression_node> m_expression;
  const sema_function& m_function;
};

class enum_node : public sema_node
{
public:
  explicit enum_node(const ast::ast_node& ast_node, lexer::token name,
                     std::vector<lexer::token> enumerators)
    : sema_node{ ast_node }
    , m_name{ name }
    , m_enumerators{ std::move(enumerators) }
  {
  }

  const lexer::token& name() const { return m_name; }
  const std::vector<lexer::token>& enumerators() const
  {
    return m_enumerators;
  }

  VISIT_METHOD

private:
  lexer::token m_name;
  std::vector<lexer::token> m_enumerators;
};

class import_node : public sema_node
{
public:
  explicit import_node(const ast::ast_node& ast_node,
                       const lexer::token& file_path)
    : sema_node{ ast_node }
    , m_file_path{ file_path }
  {
  }

  const lexer::token& file_path() const { return m_file_path; }
  cmsl::string_view pretty_file_path() const
  {
    return cmsl::string_view{ m_file_path.str().data() + 1u,
                              m_file_path.str().size() - 2u };
  }

  VISIT_METHOD

private:
  lexer::token m_file_path;
};
}
