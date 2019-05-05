#pragma once

#include "ast/type_representation.hpp"
#include "lexer/token.hpp"
#include "sema/function_signature.hpp"
#include "sema/sema_context_impl.hpp"

#include <memory>
#include <vector>

namespace cmsl {
namespace errors {
class errors_observer;
}

namespace sema {
class builtin_token_provider;
class sema_context;
class sema_context_impl;
class user_sema_function;
enum class builtin_function_kind;
class builtin_sema_function;
class sema_function;
class sema_type;
struct member_info;
class type_builder;

template <typename StoredType>
class factory
{
protected:
  template <typename TypeImplementation, typename... Args>
  TypeImplementation& create_impl(Args&&... args)
  {
    auto val =
      std::make_unique<TypeImplementation>(std::forward<Args>(args)...);
    auto ptr = val.get();
    m_storage.emplace_back(std::move(val));
    return *ptr;
  }

private:
  std::vector<std::unique_ptr<StoredType>> m_storage;
};

class sema_function_factory : public factory<sema_function>
{
public:
  ~sema_function_factory();

  user_sema_function& create_user(const sema_context& ctx,
                                  const sema_type& return_type,
                                  function_signature s);

  builtin_sema_function& create_builtin(const sema_context& ctx,
                                        const sema_type& return_type,
                                        function_signature s,
                                        builtin_function_kind kind);
};

class sema_context_factory : public factory<sema_context>
{
public:
  ~sema_context_factory();

  sema_context_impl& create(const sema_context* parent);

  sema_context_impl& create_class(const sema_context* parent);
};

class sema_type_factory : public factory<sema_type>
{
public:
  ~sema_type_factory();

  const sema_type& create(const sema_context& ctx,
                          ast::type_representation name,
                          std::vector<member_info> members);

  const sema_type& create_reference(const sema_type& referenced_type);

  const sema_type& create_homogeneous_generic(const sema_context& ctx,
                                              ast::type_representation name,
                                              const sema_type& value_type);
};

class sema_generic_type_factory : public sema_type_factory
{
public:
  explicit sema_generic_type_factory(
    sema_context& generic_types_context, const sema_context& creation_context,
    sema_type_factory& type_factory, sema_function_factory& function_factory,
    sema_context_factory& context_factory,
    errors::errors_observer& errors_observer,
    const builtin_token_provider& builtin_token_provider);

  const sema_type* create_generic(const ast::type_representation& name);

private:
  const sema_type* try_get_or_create_value_type(
    const ast::type_representation& name);

  ast::type_representation prepare_list_name_representation(
    const ast::type_representation& name) const;
  const sema_type* create_list(const ast::type_representation& name);

private:
  sema_context& m_generic_types_context;
  const sema_context& m_creation_context;
  sema_type_factory& m_type_factory;
  sema_function_factory& m_function_factory;
  sema_context_factory& m_context_factory;
  errors::errors_observer& m_errors_observer;
  const builtin_token_provider& m_builtin_token_provider;
};
}
}
