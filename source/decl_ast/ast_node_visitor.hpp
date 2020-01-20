#pragma once

namespace cmsl::decl_ast {
class bool_value_node;
class cmake_variable_access_node;
class component_declaration_node;
class component_node;
class double_value_node;
class import_node;
class int_value_node;
class list_node;
class property_access_node;
class property_append_node;
class property_node;
class string_value_node;
class translation_unit_node;

class ast_node_visitor
{
public:
  virtual ~ast_node_visitor() = default;

  virtual void visit(const bool_value_node& node) = 0;
  virtual void visit(const cmake_variable_access_node& node) = 0;
  virtual void visit(const component_declaration_node& node) = 0;
  virtual void visit(const component_node& node) = 0;
  virtual void visit(const double_value_node& node) = 0;
  virtual void visit(const import_node& node) = 0;
  virtual void visit(const int_value_node& node) = 0;
  virtual void visit(const list_node& node) = 0;
  virtual void visit(const property_access_node& node) = 0;
  virtual void visit(const property_append_node& node) = 0;
  virtual void visit(const property_node& node) = 0;
  virtual void visit(const string_value_node& node) = 0;
  virtual void visit(const translation_unit_node& node) = 0;
};
}
