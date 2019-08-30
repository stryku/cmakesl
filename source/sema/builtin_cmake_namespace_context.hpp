#pragma once

#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/function_signature.hpp"
#include "sema/sema_context_impl.hpp"

namespace cmsl::sema {
class builtin_token_provider;
class sema_function_factory;
class qualified_contextes;
class generic_type_creation_utils;

class builtin_cmake_namespace_context : public sema_context_impl
{
public:
  explicit builtin_cmake_namespace_context(
    const sema_context& parent, qualified_contextes& qualified_ctxs,
    sema_function_factory& function_factory,
    const builtin_token_provider& builtin_token_provider,
    const builtin_types_accessor& builtin_types,
    generic_type_creation_utils& generics_creation_utils);

private:
  using token_type_t = lexer::token_type;

  void add_functions();
  void add_identifiers();

  const sema_type& get_or_create_generic_type(
    const ast::type_representation& type_representation);

private:
  qualified_contextes& m_qualified_ctxs;
  sema_function_factory& m_function_factory;
  const builtin_token_provider& m_builtin_tokens;
  const builtin_types_accessor& m_builtin_types;
  generic_type_creation_utils& m_generics_creation_utils;
};
}
