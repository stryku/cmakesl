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
class add_subdirectory_handler;
class builtin_token_provider;
class identifiers_context;
class sema_context;
class sema_node;
class sema_type_factory;
class sema_function_factory;
class sema_context_factory;
class types_context;
class functions_context;

class sema_builder
{
public:
  explicit sema_builder(sema_context& ctx, errors::errors_observer& errs,
                        identifiers_context& ids_context,
                        types_context& ty_context,
                        functions_context& functions_ctx,
                        sema_type_factory& type_factory,
                        sema_function_factory& function_factory,
                        sema_context_factory& context_factory,
                        add_subdirectory_handler& add_subdirectory_handler,
                        const builtin_token_provider& builtin_token_provider,
                        builtin_types_accessor builtin_types);

  std::unique_ptr<sema_node> build(const ast::ast_node& ast_tree);

private:
  sema_context& m_ctx;
  errors::errors_observer& m_errs;
  identifiers_context& m_ids_context;
  types_context& m_types_context;
  functions_context& m_functions_context;
  sema_type_factory& m_type_factory;
  sema_function_factory& m_function_factory;
  sema_context_factory& m_context_factory;
  add_subdirectory_handler& m_add_subdirectory_handler;
  const builtin_token_provider& m_builtin_token_provider;
  builtin_types_accessor m_builtin_types;
};
}
}
