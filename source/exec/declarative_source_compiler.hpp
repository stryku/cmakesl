#pragma once

#include "sema/qualified_contextes_refs.hpp"

#include <memory>

namespace cmsl {
class source_view;
class strings_container;

namespace errors {
class errors_observer;
}

namespace decl_sema {
class component_node;
}

namespace sema {
class builtin_sema_context;
class builtin_token_provider;
class factories_provider;
class sema_function;
}

namespace exec {
class compiled_declarative_source;

class declarative_source_compiler
{
public:
  explicit declarative_source_compiler(
    errors::errors_observer& errs, strings_container& strings,
    sema::factories_provider& factories_provider,
    sema::qualified_contextes_refs qualified_contextes,
    sema::builtin_sema_context& builtin_context,
    sema::builtin_token_provider& builtin_tokens);

  std::unique_ptr<compiled_declarative_source> compile(source_view source);

private:
  const sema::sema_function& create_component_creation_function(
    const decl_sema::component_node& node);

private:
  errors::errors_observer& m_errs;
  strings_container& m_strings;
  sema::factories_provider& m_factories_provider;
  sema::qualified_contextes_refs m_qualified_contextes;
  sema::builtin_sema_context& m_builtin_context;
  sema::builtin_token_provider& m_builtin_tokens;
};

}
}
