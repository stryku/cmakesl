#pragma once

#include "ast/ast_node.hpp"
#include "ast/ast_node_visitor.hpp"
#include "ast/qualified_name.hpp"
#include "ast/type_representation.hpp"
#include "lexer/token.hpp"

#include <memory>

namespace cmsl::ast {
class binary_operator_node : public ast_node
{
public:
  explicit binary_operator_node(std::unique_ptr<ast_node> lhs, token_t op,
                                std::unique_ptr<ast_node> rhs)
    : m_lhs{ std::move(lhs) }
    , m_operator{ op }
    , m_rhs{ std::move(rhs) }
  {
  }

  const ast_node& lhs() const { return *m_lhs; }

  lexer::token operator_() const { return m_operator; }

  const ast_node& rhs() const { return *m_rhs; }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  source_location begin_location() const override
  {
    return m_lhs->begin_location();
  }

  source_location end_location() const override
  {
    return m_rhs->end_location();
  }

private:
  std::unique_ptr<ast_node> m_lhs;
  token_t m_operator;
  std::unique_ptr<ast_node> m_rhs;
};

class class_member_access_node : public ast_node
{
public:
  explicit class_member_access_node(std::unique_ptr<ast_node> lhs, token_t dot,
                                    token_t member_name)
    : m_lhs{ std::move(lhs) }
    , m_dot{ dot }
    , m_member_name{ member_name }
  {
  }

  const ast_node& lhs() const { return *m_lhs; }

  lexer::token member_name() const { return m_member_name; }

  token_t dot() const { return m_dot; }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  source_location begin_location() const override
  {
    return m_lhs->begin_location();
  }

  source_location end_location() const override
  {
    return m_member_name.src_range().end;
  }

private:
  std::unique_ptr<ast_node> m_lhs;
  token_t m_dot;
  token_t m_member_name;
};

class call_node : public ast_node
{
public:
  using params_t = std::vector<std::unique_ptr<ast_node>>;

  explicit call_node(token_t name, token_t open_paren,
                     params_t parameter_nodes, token_t close_paren)
    : m_name{ std::move(name) }
    , m_open_paren{ open_paren }
    , m_parameter_nodes{ std::move(parameter_nodes) }
    , m_close_paren{ close_paren }
  {
  }

  token_t name() const { return m_name; }

  token_t open_paren() const { return m_open_paren; }

  token_t close_paren() const { return m_close_paren; }

  const params_t& param_nodes() const { return m_parameter_nodes; }

private:
  token_t m_name;
  token_t m_open_paren;
  params_t m_parameter_nodes;
  token_t m_close_paren;
};

class function_call_node : public call_node
{
public:
  explicit function_call_node(std::vector<name_with_coloncolon> names,
                              token_t open_paren, params_t params,
                              token_t close_paren)
    : call_node{ names.back().name, open_paren, std::move(params),
                 close_paren }
    , m_name_variant{ std::move(names) }
  {
  }

  // Used in case of generic type constructor call
  explicit function_call_node(type_representation generic_type_name,
                              token_t open_paren, params_t params,
                              token_t close_paren)
    : call_node{ generic_type_name.primary_name_token(), open_paren,
                 std::move(params), close_paren }
    , m_name_variant{ std::move(generic_type_name) }
  {
  }

  const std::vector<name_with_coloncolon>& names() const
  {
    return std::get<std::vector<name_with_coloncolon>>(m_name_variant);
  }

  const type_representation& generic_type_name() const
  {
    return std::get<type_representation>(m_name_variant);
  }

  bool is_generic_type_constructor_call() const
  {
    return std::holds_alternative<type_representation>(m_name_variant);
  }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  source_location begin_location() const override
  {
    return name().src_range().begin;
  }

  source_location end_location() const override
  {
    return close_paren().src_range().end;
  }

private:
  std::variant<std::vector<name_with_coloncolon>, type_representation>
    m_name_variant;
};

class member_function_call_node : public call_node
{
public:
  explicit member_function_call_node(std::unique_ptr<ast_node> lhs,
                                     token_t dot, token_t name,
                                     token_t open_paren, params_t params,
                                     token_t close_paren)
    : call_node{ name, open_paren, std::move(params), close_paren }
    , m_lhs{ std::move(lhs) }
    , m_dot{ dot }
  {
  }

  const ast_node& lhs() const { return *m_lhs; }

  token_t dot() const { return m_dot; }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  source_location begin_location() const override
  {
    return m_lhs->begin_location();
  }

  source_location end_location() const override
  {
    return close_paren().src_range().end;
  }

private:
  std::unique_ptr<ast_node> m_lhs;
  token_t m_dot;
};

class fundamental_value_node : public ast_node
{
public:
  explicit fundamental_value_node(token_t token)
    : m_token{ token }
  {
  }

  token_t token() const { return m_token; }

  source_location begin_location() const override
  {
    return m_token.src_range().begin;
  }

  source_location end_location() const override
  {
    return m_token.src_range().end;
  }

protected:
  const token_t m_token;
};

class bool_value_node : public fundamental_value_node
{
public:
  explicit bool_value_node(token_t token)
    : fundamental_value_node{ token }
  {
  }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }
};

class int_value_node : public fundamental_value_node
{
public:
  explicit int_value_node(token_t token)
    : fundamental_value_node{ token }
  {
  }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }
};

class double_value_node : public fundamental_value_node
{
public:
  explicit double_value_node(token_t token)
    : fundamental_value_node{ token }
  {
  }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }
};

class string_value_node : public ast_node
{
public:
  explicit string_value_node(std::vector<token_t> tokens,
                             cmsl::string_view view)
    : m_tokens{ std::move(tokens) }
    , m_view{ view }
  {
  }

  const std::vector<token_t>& tokens() const { return m_tokens; }

  cmsl::string_view view() const { return m_view; }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  source_location begin_location() const override
  {
    return m_tokens.front().src_range().begin;
  }

  source_location end_location() const override
  {
    return m_tokens.back().src_range().end;
  }

private:
  std::vector<token_t> m_tokens;
  cmsl::string_view m_view;
};

class id_node : public ast_node
{
public:
  using names_t = std::vector<name_with_coloncolon>;

  explicit id_node(names_t names)
    : m_names{ std::move(names) }
  {
  }

  const names_t& names() const { return m_names; }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  source_location begin_location() const override
  {
    const auto starts_with_global_scope_accessor =
      m_names.front().name.str().empty();

    if (starts_with_global_scope_accessor) {
      return m_names.front().coloncolon->src_range().begin;
    } else {
      return m_names.front().name.src_range().begin;
    }
  }

  source_location end_location() const override
  {
    return m_names.front().name.src_range().end;
  }

private:
  names_t m_names;
};

class initializer_list_node : public ast_node
{
public:
  explicit initializer_list_node(token_t open_brace,
                                 std::vector<std::unique_ptr<ast_node>> values,
                                 token_t close_brace)
    : m_open_brace{ open_brace }
    , m_values{ std::move(values) }
    , m_close_brace{ close_brace }
  {
  }

  const std::vector<std::unique_ptr<ast_node>>& values() const
  {
    return m_values;
  }

  token_t open_brace() const { return m_open_brace; }

  token_t close_brace() const { return m_close_brace; }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  source_location begin_location() const override
  {
    return m_open_brace.src_range().begin;
  }

  source_location end_location() const override
  {
    return m_close_brace.src_range().end;
  }

private:
  token_t m_open_brace;
  std::vector<std::unique_ptr<ast_node>> m_values;
  token_t m_close_brace;
};

class unary_operator : public ast_node
{
public:
  explicit unary_operator(token_t op, std::unique_ptr<ast_node> expression)
    : m_operator{ op }
    , m_expression{ std::move(expression) }
  {
  }

  const token_t& operator_() const { return m_operator; }

  const ast_node& expression() const { return *m_expression; }

  void visit(ast_node_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  source_location begin_location() const override
  {
    return m_operator.src_range().begin;
  }

  source_location end_location() const override
  {
    return m_expression->end_location();
  }

private:
  token_t m_operator;
  std::unique_ptr<ast_node> m_expression;
};
}
