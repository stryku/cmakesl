#pragma once

#include "sema/builtin_context_base.hpp"

namespace cmsl {

namespace sema {
class builtin_token_provider;
class factories_provider;
class generic_type_creation_utils;
class type_builder;
struct cmake_namespace_types_accessor;
struct qualified_contextes_refs;
}

namespace decl_sema {
class builtin_decl_namespace_context : sema::builtin_context_base
{
public:
  explicit builtin_decl_namespace_context(
    const sema_context& parent, sema::qualified_contextes_refs& qualified_ctxs,
    sema::factories_provider& factories,
    const sema::builtin_token_provider& builtin_token_provider,
    const sema::builtin_types_accessor& builtin_types,
    sema::generic_type_creation_utils& generics_creation_utils);

private:
  const sema::builtin_types_accessor& m_builtin_types;
  sema::generic_type_creation_utils& m_generics_creation_utils
};
}
}
