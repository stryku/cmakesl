#include "exec/source_compiler.hpp"
#include "ast/ast_node.hpp"
#include "ast/parser.hpp"
#include "common/source_view.hpp"
#include "exec/compiled_source.hpp"
#include "lexer/lexer.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/enum_values_context.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/qualified_contextes_refs.hpp"
#include "sema/sema_builder.hpp"
#include "sema/sema_node.hpp"
#include "sema/types_context.hpp"

namespace cmsl::exec {
source_compiler::source_compiler(
  errors::errors_observer& errors_observer,
  sema::factories_provider& factories_provider,
  sema::add_subdirectory_handler& add_subdirectory_handler,
  sema::import_handler& imports_handler,
  sema::qualified_contextes_refs& qualified_contextes,
  sema::builtin_sema_context& builtin_context,
  sema::builtin_token_provider& builtin_tokens)
  : m_errors_observer{ errors_observer }
  , m_factories_provider{ factories_provider }
  , m_add_subdirectory_handler{ add_subdirectory_handler }
  , m_imports_handler{ imports_handler }
  , m_qualified_contextes{ qualified_contextes }
  , m_builtin_context{ builtin_context }
  , m_builtin_tokens{ builtin_tokens }
{
}

std::unique_ptr<compiled_source> source_compiler::compile(source_view source)
{
  lexer::lexer lex{ m_errors_observer, source };
  const auto tokens = lex.lex();
  ast::parser parser{ m_errors_observer, source, tokens };
  auto ast_tree = parser.parse_translation_unit();
  if (!ast_tree) {
    return nullptr;
  }

  const auto builtin_types = m_builtin_context.builtin_types();

  auto& global_context =
    m_factories_provider.context_factory().create("", &m_builtin_context);
  sema::sema_builder sema_builder{ global_context,
                                   m_errors_observer,
                                   m_qualified_contextes,
                                   m_factories_provider,
                                   m_add_subdirectory_handler,
                                   m_imports_handler,
                                   m_builtin_tokens,
                                   builtin_types };
  auto sema_tree = sema_builder.build(*ast_tree);
  if (!sema_tree) {
    return nullptr;
  }

  return std::make_unique<compiled_source>(std::move(ast_tree), global_context,
                                           std::move(sema_tree), source,
                                           builtin_types);
}
}
