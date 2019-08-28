#pragma once

#include "sema/builtin_function_kind.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/function_signature.hpp"
#include "sema/sema_context_impl.hpp"

namespace cmsl::sema {
class builtin_token_provider;
class sema_function_factory;
class functions_context;

class builtin_cmake_namespace_context : public sema_context_impl
{
public:
  explicit builtin_cmake_namespace_context(
    const sema_context& parent, functions_context& functions_ctx,
    sema_function_factory& function_factory,
    const builtin_token_provider& builtin_token_provider,
    builtin_types_accessor builtin_types);

private:
  struct builtin_function_info
  {
    const sema_type& return_type;
    function_signature signature;
    builtin_function_kind kind;
  };

  void add_functions();

private:
  functions_context& m_funcitons_context;
  sema_function_factory& m_function_factory;
  const builtin_token_provider& m_token_provider;
  builtin_types_accessor m_builtin_types;
};
}
