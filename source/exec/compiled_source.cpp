#include "exec/compiled_source.hpp"
#include "ast/ast_node.hpp"
#include "decl_ast/ast_node.hpp"
#include "decl_sema/sema_nodes.hpp"
#include "sema/builtin_sema_context.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/enum_values_context.hpp"
#include "sema/identifiers_context.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_node.hpp"

namespace cmsl::exec {
compiled_source::compiled_source(std::unique_ptr<ast::ast_node> ast_tree,
                                 const sema::sema_context& global_context,
                                 std::unique_ptr<sema::sema_node> sema_tree,
                                 source_view source,
                                 sema::builtin_types_accessor builtin_types)
  : m_ast_tree{ std::move(ast_tree) }
  , m_global_context{ std::move(global_context) }
  , m_sema_tree{ std::move(sema_tree) }
  , m_source{ source }
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

sema::builtin_types_accessor compiled_source::builtin_types() const
{
  return m_builtin_types;
}
compiled_declarative_source::compiled_declarative_source(
  std::unique_ptr<decl_ast::ast_node> ast_tree,
  std::unique_ptr<decl_sema::sema_node> sema_tree, source_view source,
  sema::builtin_types_accessor builtin_types,
  const sema::sema_function& creation_function)
  : m_ast_tree{ std::move(ast_tree) }
  , m_sema_tree{ std::move(sema_tree) }
  , m_source{ source }
  , m_builtin_types{ builtin_types }
  , m_creation_function{ creation_function }
{
}

compiled_declarative_source::~compiled_declarative_source() = default;

const sema::sema_function&
compiled_declarative_source::get_target_creation_function() const
{
  return m_creation_function;
}
}
