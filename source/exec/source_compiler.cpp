#include "exec/source_compiler.hpp"
#include "ast/ast_node.hpp"
#include "ast/parser.hpp"
#include "common/source_view.hpp"
#include "exec/compiled_source.hpp"
#include "lexer/lexer.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/factories.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/sema_builder.hpp"
#include "sema/sema_node.hpp"
#include "sema/types_context.hpp"

namespace cmsl::exec {
source_compiler::source_compiler(
  errors::errors_observer& errors_observer,
  sema::sema_type_factory& type_factory,
  sema::sema_function_factory& function_factory,
  sema::sema_context_factory& context_factory,
  sema::add_subdirectory_handler& add_subdirectory_handler,
  sema::types_context& types_ctx)
  : m_errors_observer{ errors_observer }
  , m_type_factory{ type_factory }
  , m_function_factory{ function_factory }
  , m_context_factory{ context_factory }
  , m_add_subdirectory_handler{ add_subdirectory_handler }
  , m_types_ctx{ types_ctx }
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

  auto builtin_token_provider =
    std::make_unique<sema::builtin_token_provider>("");

  auto builtin_context = std::make_unique<sema::builtin_sema_context>(
    m_type_factory, m_function_factory, m_context_factory, m_errors_observer,
    *builtin_token_provider, m_types_ctx);
  const auto builtin_types = builtin_context->builtin_types();

  auto& global_context = m_context_factory.create("", builtin_context.get());

  std::unique_ptr<sema::identifiers_context> ids_ctx =
    std::make_unique<sema::identifiers_context_impl>();
  sema::sema_builder sema_builder{ global_context,
                                   m_errors_observer,
                                   *ids_ctx,
                                   m_types_ctx,
                                   m_type_factory,
                                   m_function_factory,
                                   m_context_factory,
                                   m_add_subdirectory_handler,
                                   *builtin_token_provider,
                                   builtin_types };
  auto sema_tree = sema_builder.build(*ast_tree);
  if (!sema_tree) {
    return nullptr;
  }

  return std::make_unique<compiled_source>(
    std::move(ast_tree), std::move(builtin_context), global_context,
    std::move(sema_tree), source, std::move(builtin_token_provider),
    std::move(ids_ctx), builtin_types);
}
}
