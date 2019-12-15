#pragma once

#include "ast/ast_node_visitor.hpp"
#include "sema/builtin_types_accessor.hpp"

#include <memory>

namespace cmsl {
namespace ast {
class ast_node;
}

namespace errors {
class errors_observer;
}

namespace sema {
class add_declarative_file_semantic_handler;
class add_subdirectory_semantic_handler;
class builtin_token_provider;
class factories_provider;
class import_handler;
class sema_context;
class sema_node;
struct qualified_contextes_refs;

class sema_builder
{
public:
  explicit sema_builder(
    sema_context& ctx, errors::errors_observer& errs,
    qualified_contextes_refs& qualified_ctxs, factories_provider& factories,
    add_subdirectory_semantic_handler& add_subdirectory_handler,
    add_declarative_file_semantic_handler& add_declarative_file_handler,
    import_handler& imports_handler,
    const builtin_token_provider& builtin_token_provider,
    builtin_types_accessor builtin_types);

  std::unique_ptr<sema_node> build(const ast::ast_node& ast_tree);

private:
  sema_context& m_ctx;
  errors::errors_observer& m_errs;
  qualified_contextes_refs& m_qualified_ctxs;
  factories_provider& m_factories;
  add_subdirectory_semantic_handler& m_add_subdirectory_handler;
  add_declarative_file_semantic_handler& m_add_declarative_file_handler;
  import_handler& m_imports_handler;
  const builtin_token_provider& m_builtin_token_provider;
  builtin_types_accessor m_builtin_types;
};
}
}
