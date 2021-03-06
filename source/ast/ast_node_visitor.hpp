#pragma once

namespace cmsl::ast {
class block_node;
class class_node;
class conditional_node;
class if_else_node;
class binary_operator_node;
class class_member_access_node;
class function_call_node;
class member_function_call_node;
class bool_value_node;
class int_value_node;
class double_value_node;
class string_value_node;
class id_node;
class return_node;
class translation_unit_node;
class user_function_node;
class variable_declaration_node;
class standalone_variable_declaration_node;
class while_node;
class initializer_list_node;
class for_node;
class break_node;
class namespace_node;
class ternary_operator_node;
class designated_initializers_node;
class unary_operator;
class enum_node;
class import_node;

class ast_node_visitor
{
public:
  virtual ~ast_node_visitor() = default;

  virtual void visit(const block_node& node) = 0;
  virtual void visit(const class_node& node) = 0;
  virtual void visit(const conditional_node& node) = 0;
  virtual void visit(const if_else_node& node) = 0;
  virtual void visit(const binary_operator_node& node) = 0;
  virtual void visit(const class_member_access_node& node) = 0;
  virtual void visit(const function_call_node& node) = 0;
  virtual void visit(const member_function_call_node& node) = 0;
  virtual void visit(const bool_value_node& node) = 0;
  virtual void visit(const int_value_node& node) = 0;
  virtual void visit(const double_value_node& node) = 0;
  virtual void visit(const string_value_node& node) = 0;
  virtual void visit(const id_node& node) = 0;
  virtual void visit(const return_node& node) = 0;
  virtual void visit(const translation_unit_node& node) = 0;
  virtual void visit(const user_function_node& node) = 0;
  virtual void visit(const variable_declaration_node& node) = 0;
  virtual void visit(const standalone_variable_declaration_node& node) = 0;
  virtual void visit(const while_node& node) = 0;
  virtual void visit(const initializer_list_node& node) = 0;
  virtual void visit(const for_node& node) = 0;
  virtual void visit(const break_node& node) = 0;
  virtual void visit(const namespace_node& node) = 0;
  virtual void visit(const ternary_operator_node& node) = 0;
  virtual void visit(const designated_initializers_node& node) = 0;
  virtual void visit(const unary_operator& node) = 0;
  virtual void visit(const enum_node& node) = 0;
  virtual void visit(const import_node& node) = 0;
};
}
