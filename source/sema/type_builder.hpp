#pragma once

#include "ast/type_representation.hpp"

#include "sema/function_signature.hpp"
#include "sema_context.hpp"

namespace cmsl::sema {
struct member_info;
class sema_context_impl;
class sema_context;
class sema_type_factory;
class sema_function_factory;
class sema_context_factory;
enum class builtin_function_kind;

class type_builder
{
public:
  struct builtin_function_info
  {
    const sema_type& return_type;
    function_signature signature;
    builtin_function_kind kind;
  };

  explicit type_builder(sema_type_factory& type_factory,
                        sema_function_factory& function_factory,
                        sema_context_factory& context_factory,
                        sema_context& parent_ctx,
                        ast::type_representation name,
                        sema_context::context_type ctx_type =
                          sema::sema_context::context_type::class_);

  type_builder& with_member(const member_info& member);
  type_builder& with_user_function(const sema_type& return_type,
                                   function_signature s);
  type_builder& with_builtin_function(const sema_type& return_type,
                                      function_signature s,
                                      builtin_function_kind kind);

  template <typename Functions>
  type_builder& with_builtin_functions(Functions&& functions)
  {
    for (const auto& fun : functions) {
      with_builtin_function(fun.return_type, fun.signature, fun.kind);
    }

    return *this;
  }

  const sema_type& build_and_register_in_context();
  const sema_type& build_enum_and_register_in_context(
    std::vector<lexer::token> enumerators);
  const sema_type& build_builtin_and_register_in_context();
  const sema_type& build_homogeneous_generic_and_register_in_context(
    const sema_type& value_type);

  const sema_context& context();

  struct built_type_info
  {
    const sema_type& ty;
    const sema_type& reference;
  };
  built_type_info built_type() const;

private:
  sema_type_factory& m_type_factory;
  sema_function_factory& m_function_factory;
  sema_context_factory& m_context_factory;
  sema_context& m_current_ctx;
  sema_context& m_type_ctx;
  ast::type_representation m_name;
  std::vector<member_info> m_members;
  const sema_type* m_built_type;
  const sema_type* m_built_type_ref;
};
}
