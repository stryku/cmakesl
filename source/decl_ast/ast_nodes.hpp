#pragma once

#include "common/assert.hpp"
#include "decl_ast/ast_node.hpp"
#include "decl_ast/ast_node_visitor.hpp"

#include <memory>

#undef VISIT_MEHTOD
#define VISIT_MEHTOD                                                          \
  void visit(ast_node_visitor& visitor) const override                        \
  {                                                                           \
    visitor.visit(*this);                                                     \
  }

namespace cmsl::decl_ast {
class translation_unit_node : public ast_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<ast_node>>;

  explicit translation_unit_node(nodes_t nodes)
    : m_nodes{ std::move(nodes) }
  {
  }

  const nodes_t& nodes() const { return m_nodes; }

  source_location begin_location() const override
  {
    CMSL_ASSERT(!m_nodes.empty());
    return m_nodes.front()->begin_location();
  }

  source_location end_location() const override
  {
    CMSL_ASSERT(!m_nodes.empty());
    return m_nodes.back()->end_location();
  }

  VISIT_MEHTOD

private:
  nodes_t m_nodes;
};

class component_declaration_node : public ast_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<ast_node>>;

  explicit component_declaration_node(
    const token_t& component_token, const token_t& name_token,
    std::optional<token_t> colon_token,
    std::optional<token_t> derived_type_name_token,
    const token_t& open_brace_token, nodes_t nodes,
    const token_t& close_brace_token)
    : m_component{ component_token }
    , m_name{ name_token }
    , m_colon{ colon_token }
    , m_derived_type_name{ derived_type_name_token }
    , m_open_brace{ open_brace_token }
    , m_nodes{ std::move(nodes) }
    , m_close_brace{ close_brace_token }
  {
  }

  const token_t& component() const { return m_component; }
  const token_t& name() const { return m_name; }
  std::optional<token_t> colon() const { return m_colon; }
  std::optional<token_t> derived_type_name() const
  {
    return m_derived_type_name;
  }

  const token_t& open_brace() const { return m_open_brace; }
  const nodes_t& nodes() const { return m_nodes; }
  const token_t& close_brace() const { return m_close_brace; }

  source_location begin_location() const override
  {
    return m_open_brace.src_range().begin;
  }

  source_location end_location() const override
  {
    return m_close_brace.src_range().begin;
  }

  VISIT_MEHTOD

private:
  token_t m_component;
  token_t m_name;
  std::optional<token_t> m_colon;
  std::optional<token_t> m_derived_type_name;
  token_t m_open_brace;
  nodes_t m_nodes;
  token_t m_close_brace;
};

class component_node : public ast_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<ast_node>>;

  explicit component_node(const token_t& name,
                          std::optional<token_t> derived_type_name,
                          const token_t& open_brace, nodes_t nodes,
                          const token_t& close_brace)
    : m_name{ name }
    , m_open_brace{ open_brace }
    , m_nodes{ std::move(nodes) }
    , m_close_brace{ close_brace }
  {
  }

  const token_t& name() const { return m_name; }

  std::optional<token_t> derived_type_name() const
  {
    return m_derived_type_name;
  }

  const token_t& open_brace() const { return m_open_brace; }

  const nodes_t& nodes() const { return m_nodes; }

  const token_t& close_brace() const { return m_close_brace; }

  source_location begin_location() const override
  {
    return m_name.src_range().begin;
  }

  source_location end_location() const override
  {
    return m_close_brace.src_range().end;
  }

  VISIT_MEHTOD

private:
  token_t m_name;
  std::optional<token_t> m_derived_type_name;
  token_t m_open_brace;
  nodes_t m_nodes;
  token_t m_close_brace;
};

class property_access_node : public ast_node
{
public:
  explicit property_access_node(std::vector<token_t> property_access)
    : m_property_access{ std::move(property_access) }
  {
  }

  const std::vector<token_t>& property_access() const
  {
    return m_property_access;
  }

  source_location begin_location() const override
  {
    return m_property_access.front().src_range().begin;
  }

  source_location end_location() const override
  {
    return m_property_access.back().src_range().end;
  }

  VISIT_MEHTOD

private:
  std::vector<token_t> m_property_access;
};

class base_property_node : public ast_node
{
public:
  explicit base_property_node(
    std::unique_ptr<property_access_node> property_access,
    std::unique_ptr<ast_node> value)
    : m_property_access{ std::move(property_access) }
    , m_value{ std::move(value) }
  {
  }

  const property_access_node& property_access() const
  {
    return *m_property_access;
  }
  const ast_node& value() const { return *m_value; }

  source_location begin_location() const override
  {
    return m_property_access->begin_location();
  }

  source_location end_location() const override
  {
    return m_value->end_location();
  }

private:
  std::unique_ptr<property_access_node> m_property_access;
  std::unique_ptr<ast_node> m_value;
};

class property_node : public base_property_node
{
public:
  explicit property_node(std::unique_ptr<property_access_node> property_access,
                         const token_t& assignment,
                         std::unique_ptr<ast_node> value)
    : base_property_node{ std::move(property_access), std::move(value) }
    , m_assignment{ assignment }
  {
  }

  const token_t& assignment() const { return m_assignment; }

  VISIT_MEHTOD

private:
  token_t m_assignment;
};

class property_append_node : public base_property_node
{
public:
  explicit property_append_node(
    std::unique_ptr<property_access_node> property_access,
    const token_t& assignment, std::unique_ptr<ast_node> value)
    : base_property_node{ std::move(property_access), std::move(value) }
    , m_assignment{ assignment }
  {
  }

  const token_t& assignment() const { return m_assignment; }

  VISIT_MEHTOD

private:
  token_t m_assignment;
};

class fundamental_value_node : public ast_node
{
public:
  explicit fundamental_value_node(const token_t& token)
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
  explicit bool_value_node(const token_t& token)
    : fundamental_value_node{ token }
  {
  }

  VISIT_MEHTOD
};

class int_value_node : public fundamental_value_node
{
public:
  explicit int_value_node(const token_t& token)
    : fundamental_value_node{ token }
  {
  }

  VISIT_MEHTOD
};

class double_value_node : public fundamental_value_node
{
public:
  explicit double_value_node(const token_t& token)
    : fundamental_value_node{ token }
  {
  }

  VISIT_MEHTOD
};

class string_value_node : public fundamental_value_node
{
public:
  explicit string_value_node(const token_t& token)
    : fundamental_value_node{ token }
  {
  }

  VISIT_MEHTOD
};

class list_node : public ast_node
{
public:
  using nodes_t = std::vector<std::unique_ptr<ast_node>>;

  explicit list_node(const token_t& open_square, nodes_t nodes,
                     const token_t& close_square)
    : m_open_square{ open_square }
    , m_nodes{ std::move(nodes) }
    , m_close_square{ close_square }
  {
  }

  const token_t& open_square() const { return m_open_square; }
  const nodes_t& nodes() const { return m_nodes; }
  const token_t& close_square() const { return m_close_square; }

  source_location begin_location() const override
  {
    return m_open_square.src_range().begin;
  }

  source_location end_location() const override
  {
    return m_close_square.src_range().end;
  }

  VISIT_MEHTOD

private:
  token_t m_open_square;
  nodes_t m_nodes;
  token_t m_close_square;
};

class cmake_variable_access_node : public ast_node
{
public:
  explicit cmake_variable_access_node(const token_t& cmake_variables_token,
                                      const token_t& variable_name_token,
                                      const token_t& as_type_token)
    : m_cmake_variables{ cmake_variables_token }
    , m_variable_name{ variable_name_token }
    , m_as_type{ as_type_token }
  {
  }

  const token_t& variable_name() const { return m_variable_name; }
  const token_t& as_type() const { return m_as_type; }

  source_location begin_location() const override
  {
    return m_cmake_variables.src_range().begin;
  }

  source_location end_location() const override
  {
    return m_as_type.src_range().end;
  }

  VISIT_MEHTOD

private:
  token_t m_cmake_variables;
  token_t m_variable_name;
  token_t m_as_type;
};

class import_node : public ast_node
{
public:
  explicit import_node(const token_t& import_token,
                       const token_t& file_name_token,
                       const token_t& semicolon_token)
    : m_import{ import_token }
    , m_file_name{ file_name_token }
    , m_semicolon{ semicolon_token }
  {
  }

  const token_t& import_() const { return m_import; }
  const token_t& file_name() const { return m_file_name; }
  cmsl::string_view pretty_file_name() const
  {
    return cmsl::string_view{ m_file_name.str().data() + 1u,
                              m_file_name.str().size() - 2u };
  }
  const token_t& semicolon() const { return m_semicolon; }

  source_location begin_location() const override
  {
    return m_import.src_range().begin;
  }

  source_location end_location() const override
  {
    return m_semicolon.src_range().end;
  }

  VISIT_MEHTOD

private:
  token_t m_import;
  token_t m_file_name;
  token_t m_semicolon;
};

}
