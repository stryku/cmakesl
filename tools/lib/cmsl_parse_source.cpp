#include "cmsl_parse_source.hpp"
#include "cmsl_parsed_source.hpp"

#include "ast/ast_node.hpp"
#include "ast/parser.hpp"
#include "lexer/lexer.hpp"
#include "sema/add_subdirectory_handler.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/functions_context.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/sema_builder.hpp"
#include "sema/sema_function.hpp"
#include "sema/types_context.hpp"

namespace cmsl::sema::details {
class add_subdir_handler : public add_subdirectory_handler
{
public:
  const sema_function* handle_add_subdirectory(
    cmsl::string_view name,
    const std::vector<std::unique_ptr<expression_node>>& params) override
  {
    return nullptr;
  }
};
}

cmsl_parsed_source* cmsl_parse_source(
  const char* source, const char* builtin_types_documentation_path)
{
  auto parsed_source = new cmsl_parsed_source;
  parsed_source->source = source;
  parsed_source->add_subdirectory_handler =
    std::make_unique<cmsl::sema::details::add_subdir_handler>();

  auto builtin_documentation_path = builtin_types_documentation_path != nullptr
    ? std::string{ builtin_types_documentation_path }
    : std::string{};
  parsed_source->builtin_token_provider =
    std::make_unique<cmsl::sema::builtin_token_provider>(
      std::move(builtin_documentation_path));

  cmsl::source_view source_view{ parsed_source->source };

  cmsl::lexer::lexer lex{ parsed_source->context.errors_observer,
                          source_view };
  const auto tokens = lex.lex();
  cmsl::ast::parser parser{ parsed_source->context.errors_observer,
                            source_view, tokens };
  auto ast_tree = parser.parse_translation_unit();

  auto builtin_ctx = std::make_unique<cmsl::sema::builtin_sema_context>(
    parsed_source->context.type_factory,
    parsed_source->context.function_factory,
    parsed_source->context.context_factory,
    parsed_source->context.errors_observer,
    *(parsed_source->builtin_token_provider),
    parsed_source->context.types_ctx);

  auto builtin_types = builtin_ctx->builtin_types();

  parsed_source->builtin_context = std::move(builtin_ctx);

  auto& global_context = parsed_source->context.context_factory.create(
    "", parsed_source->builtin_context.get());

  cmsl::sema::identifiers_context_impl ids_ctx;
  cmsl::sema::types_context_impl types_context;
  cmsl::sema::functions_context_impl functions_ctx;
  cmsl::sema::sema_builder sema_builder{
    global_context,
    parsed_source->context.errors_observer,
    ids_ctx,
    types_context,
    functions_ctx,
    parsed_source->context.type_factory,
    parsed_source->context.function_factory,
    parsed_source->context.context_factory,
    *parsed_source->add_subdirectory_handler,
    *(parsed_source->builtin_token_provider),
    builtin_types
  };

  auto sema_tree = sema_builder.build(*ast_tree);

  parsed_source->ast_tree = std::move(ast_tree);
  parsed_source->sema_tree = std::move(sema_tree);

  return parsed_source;
}

void cmsl_destroy_parsed_source(cmsl_parsed_source* parsed_source)
{
  delete parsed_source;
}
