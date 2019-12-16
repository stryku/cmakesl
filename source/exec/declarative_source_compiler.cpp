#include "exec/declarative_source_compiler.hpp"
#include "common/source_view.hpp"
#include "decl_ast/ast_nodes.hpp"
#include "decl_ast/parser.hpp"
#include "decl_sema/component_creation_sema_function.hpp"
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
  sema::builtin_token_provider& builtin_tokens)
  : m_errs{ errs }
  , m_strings{ strings }
  , m_factories_provider{ factories_provider }
  , m_qualified_contextes{ qualified_contextes }
  , m_builtin_context{ builtin_context }
  , m_builtin_tokens{ builtin_tokens }
{
}

std::unique_ptr<compiled_declarative_source>
declarative_source_compiler::compile(source_view source)
{
  lexer::lexer lex{ m_errs, source };
  const auto tokens = lex.lex();
  decl_ast::parser parser{ m_errs, m_strings, source, tokens };
  auto ast_tree = parser.parse_component();
  if (!ast_tree) {
    return nullptr;
  }

  const auto builtin_types = m_builtin_context.builtin_types();

  auto& global_context =
    m_factories_provider.context_factory().create("", &m_builtin_context);

  decl_sema::sema_builder_ast_visitor_members members{ m_errs,
                                                       m_builtin_context,
                                                       global_context,
                                                       m_factories_provider,
                                                       m_builtin_tokens,
                                                       builtin_types,
                                                       m_qualified_contextes,
                                                       m_strings };

  decl_sema::sema_builder_ast_visitor sema_building_visitor{ members };
  ast_tree->visit(sema_building_visitor);

  auto sema_tree = std::move(sema_building_visitor.m_result_node);
  if (!sema_tree) {
    return nullptr;
  }

  const auto component_node =
    dynamic_cast<const decl_sema::component_node*>(sema_tree.get());
  CMSL_ASSERT(component_node);

  const auto& creation_function =
    create_component_creation_function(*component_node);

  return std::make_unique<compiled_declarative_source>(
    std::move(ast_tree), std::move(sema_tree), source, builtin_types,
    creation_function);
}

const sema::sema_function&
declarative_source_compiler::create_component_creation_function(
  const decl_sema::component_node& node)
{
  const auto& component_ty = component_type(node);
  const auto name_token =
    lexer::make_token(lexer::token_type::identifier, "__component_creation");
  auto signature = sema::function_signature{ name_token };

  auto function =
    std::make_unique<decl_sema::component_creation_sema_function>(
      m_builtin_context, component_ty, std::move(signature), node);

  return m_factories_provider.function_factory().store(std::move(function));
}

const sema::sema_type& declarative_source_compiler::component_type(
  const decl_sema::component_node& node)
{
  if (node.name().str() == "StaticLibrary") {
    return m_builtin_context.builtin_types().cmake->library;
  }
  if (node.name().str() == "Executable") {
    return m_builtin_context.builtin_types().cmake->executable;
  }

  CMSL_UNREACHABLE("Unknown component type");
}
}
