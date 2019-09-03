#include "sema/type_builder.hpp"
#include "sema/builtin_sema_function.hpp"
#include "sema/factories.hpp"
#include "sema/factories_provider.hpp"
#include "sema/sema_context_impl.hpp"
#include "sema/user_sema_function.hpp"

namespace cmsl::sema {
type_builder::type_builder(factories_provider& factories,
                           types_context& types_ctx, sema_context& parent_ctx,
                           ast::type_representation name,
                           sema_context::context_type ctx_type)
  : m_factories{ factories }
  , m_types_ctx{ types_ctx }
  , m_current_ctx{ parent_ctx }
  , m_type_ctx{ m_factories.context_factory().create(name.unqualified_name(),
                                                     &parent_ctx, ctx_type) }
  , m_name{ std::move(name) }
{
}

type_builder& type_builder::with_member(const member_info& member)
{
  m_members.emplace_back(member);
  return *this;
}

type_builder& type_builder::with_user_function(const sema_type& return_type,
                                               function_signature s)
{
  const auto& function = m_factories.function_factory().create_user(
    m_type_ctx, &return_type, std::move(s));
  m_type_ctx.add_function(function);
  return *this;
}

type_builder& type_builder::with_builtin_function(const sema_type& return_type,
                                                  function_signature s,
                                                  builtin_function_kind kind)
{
  const auto& function = m_factories.function_factory().create_builtin(
    m_type_ctx, return_type, std::move(s), kind);
  m_type_ctx.add_function(function);
  return *this;
}

const sema_type& type_builder::build_and_register_in_context(
  sema_type::flags_t flags)
{
  auto factory = m_factories.type_factory(m_types_ctx);
  const auto& type =
    factory.create(m_type_ctx, m_name, std::move(m_members), flags);
  const auto& reference_type = factory.create_reference(type);
  m_current_ctx.add_type(type);
  m_current_ctx.add_type(reference_type);

  m_built_type = &type;
  m_built_type_ref = &reference_type;

  return type;
}

const sema_type& type_builder::build_enum_and_register_in_context(
  std::vector<lexer::token> enumerators, sema_type::flags_t additional_flags)
{
  auto factory = m_factories.type_factory(m_types_ctx);
  const auto& type = factory.create_enum(
    m_type_ctx, m_name, std::move(enumerators), additional_flags);
  const auto& reference_type = factory.create_reference(type);
  m_current_ctx.add_type(type);
  m_current_ctx.add_type(reference_type);

  m_built_type = &type;
  m_built_type_ref = &reference_type;

  return type;
}

const sema_type& type_builder::build_builtin_and_register_in_context()
{
  auto factory = m_factories.type_factory(m_types_ctx);
  const auto& type =
    factory.create_builtin(m_type_ctx, m_name, std::move(m_members));
  const auto& reference_type = factory.create_reference(type);
  m_current_ctx.add_type(type);
  m_current_ctx.add_type(reference_type);

  m_built_type = &type;
  m_built_type_ref = &reference_type;

  return type;
}

const sema_type&
type_builder::build_homogeneous_generic_and_register_in_context(
  const sema_type& value_type)
{
  auto factory = m_factories.type_factory(m_types_ctx);
  const auto& type =
    factory.create_homogeneous_generic(m_type_ctx, m_name, value_type);
  const auto& reference_type = factory.create_reference(type);
  m_current_ctx.add_type(type);
  m_current_ctx.add_type(reference_type);

  m_built_type = &type;
  m_built_type_ref = &reference_type;

  return type;
}

const sema_context& type_builder::context()
{
  return m_type_ctx;
}

type_builder::built_type_info type_builder::built_type() const
{
  return { *m_built_type, *m_built_type_ref };
}
}