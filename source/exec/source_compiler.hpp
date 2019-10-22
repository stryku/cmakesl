#pragma once

#include "sema/qualified_contextes_refs.hpp"

#include <memory>

namespace cmsl {
class source_view;
class strings_container;

namespace errors {
class errors_observer;
}

namespace sema {
class add_subdirectory_semantic_handler;
class builtin_sema_context;
class builtin_token_provider;
class factories_provider;
class import_handler;
struct qualified_contextes_refs;
}

namespace exec {
class compiled_source;

class source_compiler
{
public:
  explicit source_compiler(
    errors::errors_observer& errors_observer,
    sema::factories_provider& factories_provider,
    sema::add_subdirectory_semantic_handler& add_subdirectory_handler,
    sema::import_handler& imports_handler,
    sema::qualified_contextes_refs qualified_contextes,
    sema::builtin_sema_context& builtin_context,
    sema::builtin_token_provider& builtin_tokens,
    strings_container& strings_container);

  std::unique_ptr<compiled_source> compile(source_view source);

private:
  errors::errors_observer& m_errors_observer;
  sema::factories_provider& m_factories_provider;
  sema::add_subdirectory_semantic_handler& m_add_subdirectory_handler;
  sema::import_handler& m_imports_handler;
  sema::qualified_contextes_refs m_qualified_contextes;
  sema::builtin_sema_context& m_builtin_context;
  sema::builtin_token_provider& m_builtin_tokens;
  strings_container& m_strings_container;
};
}
}
