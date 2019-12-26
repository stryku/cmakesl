#pragma once

#include "sema/builtin_context_base.hpp"
#include "sema/builtin_types_accessor.hpp"

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
struct decl_namespace_types_accessor;

class builtin_decl_namespace_context : sema::builtin_context_base
{
public:
  explicit builtin_decl_namespace_context(
    const sema_context& parent, sema::qualified_contextes_refs& qualified_ctxs,
    sema::factories_provider& factories,
    const sema::builtin_token_provider& builtin_token_provider,
    const sema::builtin_types_accessor& builtin_types,
    sema::generic_type_creation_utils& generics_creation_utils);
  ~builtin_decl_namespace_context() override;

private:
  using token_type_t = lexer::token_type;

  void add_types();
  void add_functions();

  sema::type_builder add_forwarding_lists_type();
  const sema::sema_type& add_product_type_type();
  sema::type_builder add_product_type(
    const sema::sema_type& product_type_type,
    const sema::sema_type& forwarding_lists_type);
  sema::type_builder add_shared_library_type(
    const sema::sema_type& product_type);
  sema::type_builder add_static_library_type(
    const sema::sema_type& product_type);
  sema::type_builder add_executable_type(const sema::sema_type& product_type);

private:
  sema::builtin_types_accessor m_builtin_types;
  sema::generic_type_creation_utils& m_generics_creation_utils;

  std::unique_ptr<decl_namespace_types_accessor> m_types_accessor;
};
}
}
