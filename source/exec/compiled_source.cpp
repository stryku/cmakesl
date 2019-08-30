#include "exec/compiled_source.hpp"
#include "ast/ast_node.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/enum_values_context.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_node.hpp"

namespace cmsl::exec {
compiled_source::compiled_source(
  std::unique_ptr<ast::ast_node> ast_tree,
  std::unique_ptr<sema::builtin_sema_context> builtin_context,
  const sema::sema_context& global_context,
  std::unique_ptr<sema::sema_node> sema_tree, source_view source,
  std::unique_ptr<sema::builtin_token_provider> builtin_token_provider,
  std::unique_ptr<sema::enum_values_context> enums_ctx,
  std::unique_ptr<sema::identifiers_context> ids_ctx,
  sema::builtin_types_accessor builtin_types)
  : m_ast_tree{ std::move(ast_tree) }
  , m_builtin_context{ std::move(builtin_context) }
  , m_global_context{ std::move(global_context) }
  , m_sema_tree{ std::move(sema_tree) }
  , m_source{ source }
  , m_builtin_token_provider{ std::move(builtin_token_provider) }
  , m_ids_ctx{ std::move(ids_ctx) }
  , m_enums_ctx{ std::move(enums_ctx) }
  , m_builtin_types{ builtin_types }
{
}

compiled_source::~compiled_source() = default;

const sema::sema_context& compiled_source::get_global_context() const
{
  return m_global_context;
}

const sema::sema_function* compiled_source::get_main() const
{
  const auto main_token =
    lexer::make_token(lexer::token_type::identifier, "main");
  const auto lookup_result = m_global_context.find_function(main_token);
  return lookup_result.front().front(); // Todo: handle no main function
}

const sema::sema_node& compiled_source::sema_tree() const
{
  return *m_sema_tree;
}

sema::identifiers_context& compiled_source::get_ids_ctx()
{
  return *m_ids_ctx;
}

sema::builtin_types_accessor compiled_source::builtin_types() const
{
  return m_builtin_types;
}

const sema::builtin_sema_context& compiled_source::builtin_ctx() const
{
  return *m_builtin_context;
}
}
