#include "indexing_visitor.hpp"
#include "ast/import_node.hpp"
#include "ast/infix_nodes.hpp"

namespace cmsl::tools {
void indexer::visit(const sema::translation_unit_node& node)
{
  m_identifiers_context.enter_local_ctx();

  for (const auto& sub_node : node.nodes()) {
    sub_node->visit(*this);
  }

  m_identifiers_context.leave_ctx();
}

void indexer::visit(const sema::variable_declaration_node& node)
{
  expected_type = &node.type();
  const auto& ast_node = node.ast_node();
  const auto& variable_decl =
    *dynamic_cast<const ast::variable_declaration_node*>(&ast_node);

  add_type_entry(variable_decl.type(), node.type());

  const auto is_exported =
    static_cast<const ast::variable_declaration_node*>(&node.ast_node())
      ->export_()
      .has_value();

  m_identifiers_context.register_identifier(
    variable_decl.name(), { node.type(), node.index() }, is_exported);

  if (node.initialization()) {
    node.initialization()->visit(*this);
  }
}

void indexer::visit(const sema::bool_value_node& node)
{
  // Do nothing.
}

void indexer::visit(const sema::int_value_node& node)
{
  // Do nothing.
}

void indexer::visit(const sema::double_value_node& node)
{
  // Do nothing.
}

void indexer::visit(const sema::string_value_node& node)
{
  // Do nothing.
}

void indexer::visit(const sema::id_node& node)
{
  const auto declaration_node =
    *m_identifiers_context.declaration_token_of(node.names());

  const auto id_token = node.names().back().name;
  add_entry(id_token, cmsl_index_entry_type::identifier,
            declaration_node.source().path(),
            declaration_node.src_range().begin.absolute);
}

void indexer::visit(const sema::return_node& node)
{
  node.expression().visit(*this);
}

void indexer::visit(const sema::binary_operator_node& node)
{
  node.lhs().visit(*this);
  node.rhs().visit(*this);

  const auto& operator_signature = node.operator_function().signature();
  add_entry(node.op(), cmsl_index_entry_type::operator_function,
            operator_signature.name.source().path(),
            operator_signature.name.src_range().begin.absolute);
}

void indexer::visit(const sema::function_call_node& node)
{
  visit_call_node(node);
}

void indexer::visit(const sema::member_function_call_node& node)
{
  node.lhs().visit(*this);
  visit_call_node(node);
}

void indexer::visit(const sema::implicit_member_function_call_node& node)
{
  visit_call_node(node);
}

void indexer::visit(const sema::constructor_call_node& node)
{
  visit_call_node(node);
}

void indexer::visit(const sema::block_node& node)
{
  m_identifiers_context.enter_local_ctx();

  for (const auto& sub_node : node.nodes()) {
    sub_node->visit(*this);
  }

  m_identifiers_context.leave_ctx();
}

void indexer::visit(const sema::function_node& node)
{
  m_identifiers_context.enter_local_ctx();
  const auto function_node =
    dynamic_cast<const ast::user_function_node*>(&node.ast_node());

  add_type_entry(function_node->return_type_representation(),
                 node.return_type());

  const auto& ast_param_declarations = function_node->param_declarations();
  const auto& params = node.signature().params;
  for (auto i = 0u; i < params.size(); ++i) {
    add_type_entry(ast_param_declarations[i].ty, params[i].ty);
    m_identifiers_context.register_identifier(
      params[i].name, { params[i].ty, params[i].index },
      node.ast_node().is_exported());
  }

  node.body().visit(*this);

  m_identifiers_context.leave_ctx();
}

void indexer::visit(const sema::class_node& node)
{
  m_identifiers_context.enter_global_ctx(node.name(),
                                         node.ast_node().is_exported());

  for (const auto& member : node.members()) {
    member->visit(*this);
  }

  for (const auto& function : node.functions()) {
    function->visit(*this);
  }

  m_identifiers_context.leave_ctx();
}

void indexer::visit(const sema::conditional_node& node)
{
  node.get_condition().visit(*this);
  node.get_body().visit(*this);
}

void indexer::visit(const sema::while_node& node)
{
  node.condition().visit(*this);
}

void indexer::visit(const sema::if_else_node& node)
{
  for (const auto& if_ : node.ifs()) {
    if_->visit(*this);
  }

  if (const auto else_body = node.else_body()) {
    else_body->visit(*this);
  }
}

void indexer::visit(const sema::class_member_access_node& node)
{
  const auto& lhs = node.lhs();
  lhs.visit(*this);

  const auto member = lhs.type().find_member(node.member_name().str());
  add_entry(
    node.member_access_name(), cmsl_index_entry_type::class_member_identifier,
    member->name.source().path(), member->name.src_range().begin.absolute);
}

void indexer::visit(const sema::cast_to_reference_node& node)
{
  node.expression().visit(*this);
}

void indexer::visit(const sema::cast_to_value_node& node)
{
  node.expression().visit(*this);
}

void indexer::visit(const sema::initializer_list_node& node)
{
  for (const auto& sub_node : node.values()) {
    sub_node->visit(*this);
  }
}

void indexer::visit(const sema::add_subdirectory_node& node)
{
  const auto& ast_dir_name_node =
    dynamic_cast<const ast::string_value_node&>(node.dir_name().ast_node());
  for (const auto& token : ast_dir_name_node.tokens()) {
    add_add_subdirectory_entry(token, node.dir_name().value());
  }

  visit_call_node(node);
}

void indexer::visit(const sema::unary_operator_node& node)
{
  node.expression().visit(*this);

  const auto& operator_signature = node.function().signature();
  add_entry(node.operator_(), cmsl_index_entry_type::operator_function,
            operator_signature.name.source().path(),
            operator_signature.name.src_range().begin.absolute);
}

const std::vector<cmsl_index_entry>& indexer::result() const
{
  return m_intermediate_entries;
}

void indexer::visit_call_node(const sema::call_node& node)
{
  const auto& signature = node.function().signature();
  add_entry(node.call_name(), cmsl_index_entry_type::function_call_name,
            signature.name.source().path(),
            signature.name.src_range().begin.absolute);

  for (const auto& param : node.param_expressions()) {
    param->visit(*this);
  }
}

void indexer::add_generic_type_entry(
  const ast::type_representation& type_representation,
  const sema::sema_type& type)
{
  const auto& generic_name = type_representation.generic_name();

  const auto& type_name_token = generic_name.primary_name();
  add_entry(type_name_token, cmsl_index_entry_type::type,
            type.name().source().path(),
            type.name().src_range().begin.absolute);

  const auto& generic_type =
    *dynamic_cast<const sema::homogeneous_generic_type*>(&type);

  // Todo: handle multiple nested types when they are implemented.
  const auto& nested_representation = generic_name.nested_types[0];

  add_type_entry(nested_representation, generic_type.value_type());
}

void indexer::add_type_entry(
  const ast::type_representation& type_representation,
  const sema::sema_type& type)
{
  if (type_representation.is_generic()) {
    add_generic_type_entry(type_representation, type);
  } else {
    const auto& qual_name = type_representation.qual_name();
    add_qualified_type_access_entries(qual_name.names(), type);
  }
}

void indexer::add_qualified_type_access_entries(
  const std::vector<ast::name_with_coloncolon>& names,
  const sema::sema_type& type)
{
  std::vector<ast::name_with_coloncolon> namespaces{
    std::cbegin(names), std::prev(std::cend(names))
  };

  while (!namespaces.empty()) {
    const auto declaration_token =
      m_identifiers_context.declaration_token_of_ctx(namespaces);
    add_entry(namespaces.back().name, cmsl_index_entry_type::namespace_,
              declaration_token->source().path(),
              declaration_token->src_range().begin.absolute);

    namespaces.pop_back();
  }

  const auto& type_name_token = names.back().name;
  add_entry(type_name_token, cmsl_index_entry_type::type,
            type.name().source().path(),
            type.name().src_range().begin.absolute);
}

void indexer::add_entry(const lexer::token& entry_token,
                        cmsl_index_entry_type type,
                        string_view destination_path,
                        unsigned destination_position)
{
  const auto entry =
    make_entry(entry_token, type, destination_path, destination_position);
  m_intermediate_entries.emplace_back(entry);
}

cmsl_index_entry indexer::make_entry(const lexer::token& entry_token,
                                     cmsl_index_entry_type type,
                                     string_view destination_path,
                                     unsigned destination_position)
{
  cmsl_index_entry entry{};

  entry.begin_pos = entry_token.src_range().begin.absolute;
  entry.end_pos = entry_token.src_range().end.absolute;

  entry.type = type;

  entry.source_path = new char[destination_path.size() + 1u];
  std::strcpy(entry.source_path, destination_path.data());

  entry.position = destination_position;

  return entry;
}

void indexer::visit(const sema::implicit_return_node&)
{
  // Do nothing.
}

void indexer::visit(const sema::for_node& node)
{
  if (const auto init = node.init()) {
    init->visit(*this);
  }

  if (const auto condition = node.condition()) {
    condition->visit(*this);
  }

  if (const auto iteration = node.iteration()) {
    iteration->visit(*this);
  }

  node.body().visit(*this);
}

void indexer::visit(const sema::break_node&)
{
  // Do nothing.
}

void indexer::visit(const sema::namespace_node& node)
{
  for (const auto& name_with_colon : node.names()) {
    m_identifiers_context.enter_global_ctx(name_with_colon.name,
                                           /*exported=*/false);
  }

  for (const auto& sub_node : node.nodes()) {
    sub_node->visit(*this);
  }

  for (auto i = 0u; i < node.names().size(); ++i) {
    m_identifiers_context.leave_ctx();
  }
}

void indexer::visit(const sema::ternary_operator_node& node)
{
  node.condition().visit(*this);
  node.true_().visit(*this);
  node.false_().visit(*this);
}

void indexer::visit(const sema::designated_initializers_node& node)
{
  for (const auto& initializer : node.initializers()) {
    const auto member_info =
      expected_type->find_member(initializer.name.str());
    add_entry(initializer.name, cmsl_index_entry_type::identifier,
              member_info->name.source().path(),
              member_info->name.src_range().begin.absolute);

    initializer.init->visit(*this);
  }
}

void indexer::visit(const sema::enum_node& node)
{
  // Do nothing.
}

void indexer::visit(const sema::enum_constant_access_node& node)
{
  const auto& enum_type = *dynamic_cast<const sema::enum_type*>(&node.type());
  const auto val = node.value();
  const auto& enumerator = enum_type.enumerator(val);
  const auto name = node.names().back().name;
  add_entry(name, cmsl_index_entry_type::enum_value,
            enumerator.source().path(), enumerator.src_range().begin.absolute);

  add_type_entry(node.type().name(), enum_type);
}

void indexer::visit(const sema::import_node& node)
{
  const auto& ast_import_node =
    dynamic_cast<const ast::import_node&>(node.ast_node());

  add_file_entry(ast_import_node.file_path(), node.pretty_file_path());
}

void indexer::visit(const sema::add_subdirectory_with_old_script_node& node)
{
  const auto& ast_dir_name_node =
    dynamic_cast<const ast::string_value_node&>(node.dir_name().ast_node());
  for (const auto& token : ast_dir_name_node.tokens()) {
    add_add_subdirectory_entry(token, node.dir_name().value());
  }
}

void indexer::visit(const sema::add_declarative_file_node& node)
{
  const auto& ast_dir_name_node =
    dynamic_cast<const ast::string_value_node&>(node.file_path().ast_node());
  for (const auto& token : ast_dir_name_node.tokens()) {
    add_add_subdirectory_entry(token, node.file_path().value());
  }
}

void indexer::visit(
  const sema::add_subdirectory_with_declarative_script_node& node)
{
  const auto& ast_dir_name_node =
    dynamic_cast<const ast::string_value_node&>(node.dir_name().ast_node());
  for (const auto& token : ast_dir_name_node.tokens()) {
    add_add_subdirectory_entry(token, node.dir_name().value());
  }
}

void indexer::visit(const sema::cast_to_base_value_node& node)
{
  node.expression().visit(*this);
}

void indexer::visit(const sema::cast_to_reference_to_base_node& node)
{
  node.expression().visit(*this);
}

void indexer::add_file_entry(const lexer::token& token,
                             string_view destination_path)
{
  add_entry(token, cmsl_index_entry_type::file, destination_path, 0u);
}

void indexer::add_add_subdirectory_entry(const lexer::token& token,
                                         cmsl::string_view subdir_name)
{
  add_entry(token, cmsl_index_entry_type::add_subdirectory, subdir_name, 0);
}
}
