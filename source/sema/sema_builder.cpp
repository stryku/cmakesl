#include "sema/sema_builder.hpp"

#include "ast/ast_node.hpp"
#include "sema/sema_builder_ast_visitor.hpp"

namespace cmsl::sema {
sema_builder::sema_builder(
  sema_context& ctx, errors::errors_observer& errs,
  qualified_contextes_refs& qualified_ctxs, factories_provider& factories,
  add_subdirectory_semantic_handler& add_subdirectory_handler,
  import_handler& imports_handler,
  const builtin_token_provider& builtin_token_provider,
  builtin_types_accessor builtin_types)
  : m_ctx{ ctx }
  , m_errs{ errs }
  , m_qualified_ctxs{ qualified_ctxs }
  , m_factories{ factories }
  , m_add_subdirectory_handler{ add_subdirectory_handler }
  , m_imports_handler{ imports_handler }
  , m_builtin_token_provider{ builtin_token_provider }
  , m_builtin_types{ builtin_types }
{
}

std::unique_ptr<sema_node> sema_builder::build(const ast::ast_node& ast_tree)
{
  parsing_context parsing_ctx{};

  auto members = sema_builder_ast_visitor_members{ m_ctx, // generic types ctx
                                                   m_ctx, // global ctx
                                                   m_errs,
                                                   m_qualified_ctxs,
                                                   m_factories,
                                                   m_builtin_token_provider,
                                                   parsing_ctx,
                                                   m_builtin_types,
                                                   m_add_subdirectory_handler,
                                                   m_imports_handler };

  sema_builder_ast_visitor visitor{ members };

  ast_tree.visit(visitor);
  return std::move(visitor.m_result_node);
}
}
