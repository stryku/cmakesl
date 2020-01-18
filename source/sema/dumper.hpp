#pragma once

#include "common/dumper_utils.hpp"
#include "sema/sema_node_visitor.hpp"

#include <ast/namespace_node.hpp>
#include <ostream>

namespace cmsl::sema {

class call_node;
class sema_type;

class dumper
  : public sema_node_visitor
  , public dumper_utils
{
public:
  explicit dumper(std::ostream& out)
    : dumper_utils{ out }
  {
  }

  void visit(const add_declarative_file_node&) override;
  void visit(const add_subdirectory_node&) override;
  void visit(const add_subdirectory_with_declarative_script_node&) override;
  void visit(const add_subdirectory_with_old_script_node&) override;
  void visit(const binary_operator_node&) override;
  void visit(const block_node&) override;
  void visit(const bool_value_node&) override;
  void visit(const break_node&) override;
  void visit(const cast_to_reference_node&) override;
  void visit(const cast_to_reference_to_base_node&) override;
  void visit(const cast_to_base_value_node&) override;
  void visit(const cast_to_value_node&) override;
  void visit(const class_member_access_node&) override;
  void visit(const class_node&) override;
  void visit(const conditional_node&) override;
  void visit(const constructor_call_node&) override;
  void visit(const designated_initializers_node& node) override;
  void visit(const double_value_node&) override;
  void visit(const enum_constant_access_node& node) override;
  void visit(const enum_node& node) override;
  void visit(const for_node&) override;
  void visit(const function_call_node&) override;
  void visit(const function_node&) override;
  void visit(const id_node&) override;
  void visit(const if_else_node&) override;
  void visit(const implicit_member_function_call_node&) override;
  void visit(const implicit_return_node&) override;
  void visit(const import_node& node) override;
  void visit(const initializer_list_node&) override;
  void visit(const int_value_node&) override;
  void visit(const member_function_call_node&) override;
  void visit(const namespace_node& node) override;
  void visit(const return_node&) override;
  void visit(const string_value_node&) override;
  void visit(const ternary_operator_node& node) override;
  void visit(const translation_unit_node&) override;
  void visit(const unary_operator_node& node) override;
  void visit(const variable_declaration_node&) override;
  void visit(const while_node&) override;

private:
  void visit_call_node(const call_node& node);

  void dump_exported(const sema_node& node);
  void dump_type(const sema_type& type);
  void dump_qualified_name(
    const std::vector<ast::name_with_coloncolon>& names);
};
}
