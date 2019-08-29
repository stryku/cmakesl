#include "sema/sema_builder.hpp"

#include "ast/ast_node.hpp"
#include "sema/sema_builder_ast_visitor.hpp"

namespace cmsl::sema {
sema_builder::sema_builder(
  sema_context& ctx, errors::errors_observer& errs,
  enum_values_context& enums_context, identifiers_context& ids_context,
  types_context& ty_context, functions_context& functions_ctx,
  sema_type_factory& type_factory, sema_function_factory& function_factory,
  sema_context_factory& context_factory,
  add_subdirectory_handler& add_subdirectory_handler,
  const builtin_token_provider& builtin_token_provider,
  builtin_types_accessor builtin_types)
  : m_ctx{ ctx }
  , m_errs{ errs }
  , m_ids_context{ ids_context }
  , m_enums_context{ enums_context }
  , m_types_context{ ty_context }
  , m_functions_context{ functions_ctx }
  , m_type_factory{ type_factory }
  , m_function_factory{ function_factory }
  , m_context_factory{ context_factory }
  , m_add_subdirectory_handler{ add_subdirectory_handler }
  , m_builtin_token_provider{ builtin_token_provider }
  , m_builtin_types{ builtin_types }
{
}

std::unique_ptr<sema_node> sema_builder::build(const ast::ast_node& ast_tree)
{
  parsing_context parsing_ctx{};

  auto qualified_ctxs =
    qualified_contextes{ m_enums_context, m_functions_context, m_ids_context,
                         m_types_context };
  auto members = sema_builder_ast_visitor_members{ m_ctx, // generic types ctx
                                                   m_ctx, // global ctx
                                                   m_errs,
                                                   qualified_ctxs,
                                                   m_type_factory,
                                                   m_function_factory,
                                                   m_context_factory,
                                                   m_add_subdirectory_handler,
                                                   m_builtin_token_provider,
                                                   parsing_ctx,
                                                   m_builtin_types };

  sema_builder_ast_visitor visitor{ members };

  ast_tree.visit(visitor);
  return std::move(visitor.m_result_node);
}
}
