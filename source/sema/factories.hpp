#pragma once

#include "ast/type_representation.hpp"
#include "lexer/token.hpp"
#include "sema/builtin_types_accessor.hpp"
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
class types_context;

template <typename StoredType>
class factory
{
protected:
  using storage_t = std::vector<std::unique_ptr<StoredType>>;

  explicit factory(storage_t& storage)
    : m_storage{ storage }
  {
  }

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
  storage_t& m_storage;
};

class sema_function_factory : public factory<sema_function>
{
public:
  explicit sema_function_factory(storage_t& storage)
    : factory{ storage }
  {
  }
  ~sema_function_factory();

  user_sema_function& create_user(const sema_context& ctx,
                                  const sema_type* return_type,
                                  function_signature s);

  builtin_sema_function& create_builtin(const sema_context& ctx,
                                        const sema_type& return_type,
                                        function_signature s,
                                        builtin_function_kind kind);
};

class sema_context_factory : public factory<sema_context>
{
public:
  explicit sema_context_factory(storage_t& storage)
    : factory{ storage }
  {
  }
  ~sema_context_factory();

  sema_context_impl& create(
    std::string name, const sema_context* parent,
    sema_context::context_type type = sema_context::context_type::namespace_);

  sema_context_impl& create_class(std::string name,
                                  const sema_context* parent);

  sema_context_impl& create_enum(std::string name, const sema_context* parent);
};

class sema_type_factory : public factory<sema_type>
{
public:
  explicit sema_type_factory(types_context& types_ctx, storage_t& storage)
    : factory{ storage }
    , m_types_ctx{ types_ctx }
  {
  }

  ~sema_type_factory();

  const sema_type& create(const sema_context& ctx,
                          ast::type_representation name,
                          std::vector<member_info> members,
                          sema_type::flags_t flags = {});

  const sema_type& create_builtin(const sema_context& ctx,
                                  ast::type_representation name,
                                  std::vector<member_info> members);

  const sema_type& create_reference(const sema_type& referenced_type);

  const sema_type& create_homogeneous_generic(const sema_context& ctx,
                                              ast::type_representation name,
                                              const sema_type& value_type);

  const sema_type& create_designated_initializer(
    const sema_context& ctx, ast::type_representation name);

  const sema_type& create_enum(const sema_context& ctx,
                               ast::type_representation name,
                               std::vector<lexer::token> enumerators,
                               sema_type::flags_t additional_flags = {});

private:
  template <typename T, typename... Args>
  const sema_type& create_and_add(const lexer::token& primary_name, Args&&...);

protected:
  types_context& m_types_ctx;
};

class factories_provider;

class sema_generic_type_factory
{
public:
  explicit sema_generic_type_factory(
    sema_context& generic_types_context, const sema_context& creation_context,
    factories_provider& factories, errors::errors_observer& errors_observer,
    const builtin_token_provider& builtin_token_provider,
    const builtin_types_accessor& builtin_types, types_context& types_ctx);

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
  factories_provider& m_factories;
  types_context& m_types_ctx;
  errors::errors_observer& m_errors_observer;
  const builtin_token_provider& m_builtin_token_provider;
  const builtin_types_accessor& m_builtin_types;
};
}
}
