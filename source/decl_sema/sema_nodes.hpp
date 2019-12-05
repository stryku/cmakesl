#pragma once

#include "common/int_alias.hpp"
#include "decl_ast/ast_nodes.hpp"

namespace cmsl::decl_sema {
class sema_node
{
public:
  using token_t = lexer::token;

  explicit sema_node(const decl_ast::ast_node& ast_node)
    : m_ast_node{ ast_node }
  {
  }

  virtual ~sema_node() = default;

  virtual source_location begin_location() const
  {
    return m_ast_node.begin_location();
  }
  virtual source_location end_location() const
  {
    return m_ast_node.end_location();
  }
  const decl_ast::ast_node& ast_node() const { return m_ast_node; }

private:
  const decl_ast::ast_node& m_ast_node;
};

class expression_node : public sema_node
{
public:
  using sema_node::sema_node;
};

template <typename T>
class value_node : public expression_node
{
public:
  explicit value_node(const decl_ast::ast_node& ast_node, T val)
    : expression_node{ ast_node }
    , m_value{ val }
  {
  }

  T value() const { return m_value; }

private:
  T m_value;
};

class bool_value_node : public value_node<bool>
{
public:
  explicit bool_value_node(const decl_ast::ast_node& ast_node, bool val)
    : value_node{ ast_node, val }
  {
  }
};

class int_value_node : public value_node<int_t>
{
public:
  explicit int_value_node(const decl_ast::ast_node& ast_node, int_t val)
    : value_node{ ast_node, val }
  {
  }
};

class double_value_node : public value_node<double>
{
public:
  explicit double_value_node(const decl_ast::ast_node& ast_node, double val)
    : value_node{ ast_node, val }
  {
  }
};

class string_value_node : public value_node<cmsl::string_view>
{
public:
  explicit string_value_node(const decl_ast::ast_node& ast_node,
                             cmsl::string_view val)
    : value_node{ ast_node, val }
  {
  }
};

class list_node : public expression_node
{
public:
  using values_t = std::vector<std::unique_ptr<expression_node>>;

  explicit list_node(const decl_ast::ast_node& ast_node, values_t values)
    : expression_node{ ast_node }
    , m_values{ std::move(values) }
  {
  }

  const values_t& values() const { return m_values; }

private:
  values_t m_values;
};

class property_node : public sema_node
{
public:
  explicit property_node(const decl_ast::ast_node& ast_node,
                         const token_t& name,
                         std::unique_ptr<expression_node> value)
    : sema_node{ ast_node }
    , m_name{ name }
    , m_value{ std::move(value) }
  {
  }

  const token_t& name() const { return m_name; }
  const expression_node& value() const { return *m_value; }

private:
  token_t m_name;
  std::unique_ptr<expression_node> m_value;
};

class component_node : public sema_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<sema_node>>;

  explicit component_node(const decl_ast::ast_node& ast_node, nodes_t nodes)
    : sema_node{ ast_node }
    , m_nodes{ std::move(nodes) }
  {
  }

  const nodes_t& nodes() const { return m_nodes; }

private:
  nodes_t m_nodes;
};

}
