#pragma once

#include "ast/user_function_node.hpp"
#include "ast/variable_declaration_node.hpp"
#include "sema/enum_type.hpp"
#include "sema/homogeneous_generic_type.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/qualified_contextes_refs.hpp"
#include "sema/sema_node_visitor.hpp"
#include "sema/sema_nodes.hpp"

#include "cmsl_index.hpp"
#include "cmsl_parsed_source.hpp"

#include <cstring>

namespace cmsl::tools {
class indexer : public sema::sema_node_visitor
{
public:
  void visit(const sema::add_subdirectory_node& node) override;
  void visit(const sema::add_subdirectory_with_old_script_node& node) override;
  void visit(const sema::binary_operator_node& node) override;
  void visit(const sema::block_node& node) override;
  void visit(const sema::bool_value_node& node) override;
  void visit(const sema::break_node&) override;
  void visit(const sema::cast_to_reference_node& node) override;
  void visit(const sema::cast_to_value_node& node) override;
  void visit(const sema::class_member_access_node& node) override;
  void visit(const sema::class_node& node) override;
  void visit(const sema::conditional_node& node) override;
  void visit(const sema::constructor_call_node& node) override;
  void visit(const sema::designated_initializers_node& node) override;
  void visit(const sema::double_value_node& node) override;
  void visit(const sema::enum_constant_access_node& node) override;
  void visit(const sema::enum_node& node) override;
  void visit(const sema::for_node& node) override;
  void visit(const sema::function_call_node& node) override;
  void visit(const sema::function_node& node) override;
  void visit(const sema::id_node& node) override;
  void visit(const sema::if_else_node& node) override;
  void visit(const sema::implicit_member_function_call_node& node) override;
  void visit(const sema::implicit_return_node&) override;
  void visit(const sema::import_node& node) override;
  void visit(const sema::initializer_list_node& node) override;
  void visit(const sema::int_value_node& node) override;
  void visit(const sema::member_function_call_node& node) override;
  void visit(const sema::namespace_node& node) override;
  void visit(const sema::return_node& node) override;
  void visit(const sema::string_value_node& node) override;
  void visit(const sema::ternary_operator_node& node) override;
  void visit(const sema::translation_unit_node& node) override;
  void visit(const sema::unary_operator_node& node) override;
  void visit(const sema::variable_declaration_node& node) override;
  void visit(const sema::while_node& node) override;

  const std::vector<cmsl_index_entry>& result() const;

private:
  void visit_call_node(const sema::call_node& node);

  void add_generic_type_entry(
    const ast::type_representation& type_representation,
    const sema::sema_type& type);

  void add_type_entry(const ast::type_representation& type_representation,
                      const sema::sema_type& type);

  void add_qualified_type_access_entries(
    const std::vector<ast::name_with_coloncolon>& names,
    const sema::sema_type& type);

  void add_entry(const lexer::token& entry_token, cmsl_index_entry_type type,
                 string_view destination_path, unsigned destination_position);

  cmsl_index_entry make_entry(const lexer::token& entry_token,
                              cmsl_index_entry_type type,
                              string_view destination_path,
                              unsigned destination_position);

private:
  std::vector<cmsl_index_entry> m_intermediate_entries;
  sema::identifiers_context_impl m_identifiers_context;
  const sema::sema_type* expected_type{ nullptr };
};
}