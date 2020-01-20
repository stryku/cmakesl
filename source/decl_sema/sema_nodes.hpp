#pragma once

#include "common/assert.hpp"
#include "common/int_alias.hpp"
#include "decl_ast/ast_nodes.hpp"
#include "decl_sema/sema_node_visitor.hpp"
#include "sema/identifiers_index_provider.hpp"
#include "sema/sema_context.hpp"
#include "sema/type_member_info.hpp"

#undef VISIT_MEHTOD
#define VISIT_MEHTOD                                                          \
  void visit(sema_node_visitor& visitor) const override                       \
  {                                                                           \
    visitor.visit(*this);                                                     \
  }

namespace cmsl {
namespace sema {
class sema_type;
}

namespace decl_sema {
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

  virtual void visit(sema_node_visitor& visitor) const = 0;

private:
  const decl_ast::ast_node& m_ast_node;
};

class expression_node : public sema_node
{
public:
  using sema_node::sema_node;

  virtual const sema::sema_type& type() const = 0;
};

template <typename T>
class value_node : public expression_node
{
public:
  explicit value_node(const decl_ast::ast_node& ast_node, T val,
                      const sema::sema_type& ty)
    : expression_node{ ast_node }
    , m_value{ val }
    , m_type{ ty }
  {
  }

  T value() const { return m_value; }

  const sema::sema_type& type() const { return m_type; }

private:
  T m_value;
  const sema::sema_type& m_type;
};

class bool_value_node : public value_node<bool>
{
public:
  explicit bool_value_node(const decl_ast::ast_node& ast_node, bool val,
                           const sema::sema_type& ty)
    : value_node{ ast_node, val, ty }
  {
  }

  VISIT_MEHTOD
};

class int_value_node : public value_node<int_t>
{
public:
  explicit int_value_node(const decl_ast::ast_node& ast_node, int_t val,
                          const sema::sema_type& ty)
    : value_node{ ast_node, val, ty }
  {
  }

  VISIT_MEHTOD
};

class double_value_node : public value_node<double>
{
public:
  explicit double_value_node(const decl_ast::ast_node& ast_node, double val,
                             const sema::sema_type& ty)
    : value_node{ ast_node, val, ty }
  {
  }

  VISIT_MEHTOD
};

class string_value_node : public value_node<cmsl::string_view>
{
public:
  explicit string_value_node(const decl_ast::ast_node& ast_node,
                             cmsl::string_view val, const sema::sema_type& ty)
    : value_node{ ast_node, val, ty }
  {
  }

  VISIT_MEHTOD
};

class list_node : public expression_node
{
public:
  using values_t = std::vector<std::unique_ptr<expression_node>>;

  explicit list_node(const decl_ast::ast_node& ast_node, values_t values,
                     const sema::sema_type& ty)
    : expression_node{ ast_node }
    , m_values{ std::move(values) }
    , m_type{ ty }
  {
  }

  const values_t& values() const { return m_values; }

  const sema::sema_type& type() const override { return m_type; }

  VISIT_MEHTOD

private:
  values_t m_values;
  const sema::sema_type& m_type;
};

class base_property_node : public sema_node
{
public:
  explicit base_property_node(
    const decl_ast::ast_node& ast_node,
    std::unique_ptr<property_access_node> property_access,
    std::unique_ptr<expression_node> value)
    : sema_node{ ast_node }
    , m_property_access{ std::move(property_access) }
    , m_value{ std::move(value) }
  {
  }

  const property_access_node& property_access() const
  {
    return *m_property_access;
  }
  const expression_node& value() const { return *m_value; }

private:
  std::unique_ptr<property_access_node> m_property_access;
  std::unique_ptr<expression_node> m_value;
};

class property_node : public base_property_node
{
public:
  using base_property_node::base_property_node;

  VISIT_MEHTOD
};

class property_append_node : public base_property_node
{
public:
  using base_property_node::base_property_node;

  VISIT_MEHTOD
};

class property_access_node : public expression_node
{
public:
  explicit property_access_node(
    const decl_ast::ast_node& ast_node,
    std::vector<sema::member_info> properties_access)
    : expression_node{ ast_node }
    , m_properties_access{ std::move(properties_access) }
  {
  }

  const std::vector<sema::member_info>& properties_access() const
  {
    return m_properties_access;
  }

  const sema::sema_type& type() const override
  {
    const auto& last_type = m_properties_access.back().ty;
    const auto& ctx = last_type.context();
    const auto last_type_ref = ctx.find_reference_for(last_type);
    CMSL_ASSERT(last_type_ref);
    return *last_type_ref;
  }

  VISIT_MEHTOD

private:
  std::vector<sema::member_info> m_properties_access;
};

class component_declaration_node : public sema_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<sema_node>>;

  explicit component_declaration_node(
    const decl_ast::ast_node& ast_node, const sema::sema_type& type,
    const component_declaration_node* derived_component_declaration,
    const token_t& name, nodes_t nodes)
    : sema_node{ ast_node }
    , m_type{ type }
    , m_derived_component_declaration{ derived_component_declaration }
    , m_name{ name }
    , m_nodes{ std::move(nodes) }
  {
  }

  const sema::sema_type& type() const { return m_type; }
  const component_declaration_node* derived_component_declaration() const
  {
    return m_derived_component_declaration;
  }
  const token_t& name() const { return m_name; }
  const nodes_t& nodes() const { return m_nodes; }

  VISIT_MEHTOD

private:
  const sema::sema_type& m_type;
  const component_declaration_node* m_derived_component_declaration;
  token_t m_name;
  nodes_t m_nodes;
};

class component_node : public sema_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<sema_node>>;

  explicit component_node(
    const decl_ast::ast_node& ast_node, const sema::sema_type& type,
    const component_declaration_node* component_declaration,
    const token_t& name, nodes_t nodes)
    : sema_node{ ast_node }
    , m_type{ type }
    , m_component_declaration{ component_declaration }
    , m_name{ name }
    , m_nodes{ std::move(nodes) }
  {
  }

  const sema::sema_type& type() const { return m_type; }
  const component_declaration_node* component_declaration() const
  {
    return m_component_declaration;
  }
  const token_t& name() const { return m_name; }
  const nodes_t& nodes() const { return m_nodes; }

  VISIT_MEHTOD

private:
  const sema::sema_type& m_type;
  const component_declaration_node* m_component_declaration;
  token_t m_name;
  nodes_t m_nodes;
};

class cmake_variable_access_node : public expression_node
{
public:
  explicit cmake_variable_access_node(const decl_ast::ast_node& ast_node,
                                      const token_t& variable_name,
                                      const sema::sema_type& as_type)
    : expression_node{ ast_node }
    , m_variable_name{ variable_name }
    , m_as_type{ as_type }
  {
  }

  const token_t& variable_name() const { return m_variable_name; }
  const sema::sema_type& type() const override { return m_as_type; }

  VISIT_MEHTOD

private:
  token_t m_variable_name;
  const sema::sema_type& m_as_type;
};

class translation_unit_node : public sema_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<sema_node>>;

  explicit translation_unit_node(const decl_ast::ast_node& ast_node,
                                 nodes_t nodes)
    : sema_node{ ast_node }
    , m_nodes{ std::move(nodes) }
  {
  }

  const nodes_t& nodes() const { return m_nodes; }

  VISIT_MEHTOD

private:
  nodes_t m_nodes;
};

class import_node : public sema_node
{
public:
  explicit import_node(const decl_ast::ast_node& ast_node,
                       const token_t& file_name)
    : sema_node{ ast_node }
    , m_file_name{ file_name }
  {
  }

  const lexer::token& file_path() const { return m_file_name; }
  cmsl::string_view pretty_file_path() const
  {
    return cmsl::string_view{ m_file_name.str().data() + 1u,
                              m_file_name.str().size() - 2u };
  }

  VISIT_MEHTOD

private:
  token_t m_file_name;
};

}
}
