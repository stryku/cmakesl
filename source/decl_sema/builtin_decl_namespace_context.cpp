#include "decl_sema/builtin_decl_namespace_context.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/qualified_contextes_refs.hpp"

#include <generated/builtin_token_providers.hpp>

namespace cmsl::decl_sema {

builtin_decl_namespace_context::builtin_decl_namespace_context(
  const sema::sema_context& parent,
  sema::qualified_contextes_refs& qualified_ctxs,
  sema::factories_provider& factories,
  const sema::builtin_token_provider& builtin_token_provider,
  const sema::builtin_types_accessor& builtin_types,
  sema::generic_type_creation_utils& generics_creation_utils)
  : builtin_context_base{ qualified_ctxs, factories, builtin_token_provider,
                          "decl", &parent }
  , m_builtin_types{ builtin_types }
  , m_generics_creation_utils{ generics_creation_utils }
{
  auto guard = m_qualified_ctxs.all_qualified_ctxs_guard(
    m_builtin_tokens.decl().name(), /*exported=*/false);
}

}
