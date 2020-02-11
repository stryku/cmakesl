#include "exec/declarative_source_compiler.hpp"
#include "common/source_view.hpp"
#include "decl_ast/ast_nodes.hpp"
#include "decl_ast/parser.hpp"
#include "decl_sema/builtin_decl_namespace_context.hpp"
#include "decl_sema/component_creation_sema_function.hpp"
#include "decl_sema/declarative_import_handler.hpp"
#include "decl_sema/sema_builder_ast_visitor.hpp"
#include "decl_sema/sema_nodes.hpp"
#include "exec/compiled_source.hpp"
#include "lexer/lexer.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/cmake_namespace_types_accessor.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"

namespace cmsl::exec {
declarative_source_compiler::declarative_source_compiler(
  errors::errors_observer& errs, strings_container& strings,
  sema::factories_provider& factories_provider,
  sema::qualified_contextes_refs qualified_contextes,
  sema::builtin_sema_context& builtin_context,
  decl_sema::builtin_decl_namespace_context& decl_context,
  sema::builtin_token_provider& builtin_tokens,
  decl_sema::declarative_import_handler& import_handler)
  : m_errs{ errs }
  , m_strings{ strings }
  , m_factories_provider{ factories_provider }
  , m_qualified_contextes{ qualified_contextes }
  , m_builtin_context{ builtin_context }
  , m_decl_context{ decl_context }
  , m_builtin_tokens{ builtin_tokens }
  , m_import_handler{ import_handler }
{
}

std::unique_ptr<compiled_declarative_source>
declarative_source_compiler::compile(source_view source)
{
  lexer::lexer lex{ m_errs, source };
  const auto tokens = lex.lex();
  decl_ast::parser parser{ m_errs, m_strings, source, tokens };
  auto ast_tree = parser.parse_translation_unit();
  if (!ast_tree) {
    return nullptr;
  }

  const auto builtin_types = m_builtin_context.builtin_types();

  cmsl::string_view_map<const decl_sema::component_declaration_node*>
    component_declarations;

  decl_sema::sema_builder_ast_visitor_members members{ m_errs,
                                                       m_builtin_context,
                                                       m_decl_context,
                                                       m_factories_provider,
                                                       m_builtin_tokens,
                                                       builtin_types,
                                                       m_qualified_contextes,
                                                       m_strings,
                                                       component_declarations,
                                                       m_import_handler };

  decl_sema::sema_builder_ast_visitor sema_building_visitor{ members };
  ast_tree->visit(sema_building_visitor);

  auto sema_tree = std::move(sema_building_visitor.m_result_node);
  if (!sema_tree) {
    return nullptr;
  }

  const auto translation_unit =
    dynamic_cast<const decl_sema::translation_unit_node*>(sema_tree.get());
  CMSL_ASSERT(translation_unit);

  const sema::sema_function* creation_function{ nullptr };

  for (const auto& node : translation_unit->nodes()) {
    if (const auto component_node =
          dynamic_cast<const decl_sema::component_node*>(node.get())) {
      creation_function = &create_component_creation_function(*component_node);
    }
  }

  return std::make_unique<compiled_declarative_source>(
    std::move(ast_tree), std::move(sema_tree), source, builtin_types,
    creation_function, std::move(component_declarations));
}

const sema::sema_function&
declarative_source_compiler::create_component_creation_function(
  const decl_sema::component_node& node)
{
  const auto& component_ty = node.type();
  const auto name_token =
    lexer::make_token(lexer::token_type::identifier, "__component_creation");
  auto signature = sema::function_signature{ name_token };

  const auto& function_return_type =
    map_declarative_type_to_imperative_type(component_ty);

  auto function =
    std::make_unique<decl_sema::component_creation_sema_function>(
      m_builtin_context, function_return_type, std::move(signature), node);

  return m_factories_provider.function_factory().store(std::move(function));
}

const sema::sema_type&
declarative_source_compiler::map_declarative_type_to_imperative_type(
  const sema::sema_type& declarative_type) const
{
  const auto types = m_decl_context.types_accessor();
  const auto imperative_types = *m_builtin_context.builtin_types().cmake;
  if (declarative_type == types.static_library ||
      declarative_type.derives_from(types.static_library)) {
    return imperative_types.library;
  }
  if (declarative_type == types.shared_library ||
      declarative_type.derives_from(types.shared_library)) {
    return imperative_types.library;
  }
  if (declarative_type == types.executable ||
      declarative_type.derives_from(types.executable)) {
    return imperative_types.executable;
  }
  if (declarative_type == types.test_executable ||
      declarative_type.derives_from(types.test_executable)) {
    return imperative_types.executable;
  }

  CMSL_UNREACHABLE("Unknown component type");
}
}
