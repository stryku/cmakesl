#include "decl_sema/builtin_decl_namespace_context.hpp"
#include "decl_sema/decl_namespace_types_accessor.hpp"
#include "sema/builtin_token_provider.hpp"
#include "sema/builtin_types_accessor.hpp"
#include "sema/generic_type_creation_utils.hpp"
#include "sema/identifiers_index_provider.hpp"
#include "sema/qualified_contextes_refs.hpp"
#include "sema/type_builder.hpp"
#include "sema/type_member_info.hpp"

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

  add_types();
  add_functions();
}

builtin_decl_namespace_context::~builtin_decl_namespace_context() = default;

void builtin_decl_namespace_context::add_types()
{
  const auto forwarding_lists_manipulator = add_forwarding_lists_type();
  const auto& product_type_type = add_product_type_type();
  const auto product_type_manipulator = add_product_type(
    product_type_type, forwarding_lists_manipulator.built_type().ty);
  const auto executable_manipulator =
    add_executable_type(product_type_manipulator.built_type().ty);
  const auto static_library_manipulator =
    add_static_library_type(product_type_manipulator.built_type().ty);
  const auto shared_library_manipulator =
    add_shared_library_type(product_type_manipulator.built_type().ty);

  decl_namespace_types_accessor accessor{
    .forwarding_lists = forwarding_lists_manipulator.built_type().ty,
    .forwarding_lists_ref =
      forwarding_lists_manipulator.built_type().reference,
    .product = product_type_manipulator.built_type().ty,
    .product_ref = product_type_manipulator.built_type().reference,
    .executable = executable_manipulator.built_type().ty,
    .executable_ref = executable_manipulator.built_type().reference,
    .static_library = static_library_manipulator.built_type().ty,
    .static_library_ref = static_library_manipulator.built_type().reference,
    .shared_library = shared_library_manipulator.built_type().ty,
    .shared_library_ref = shared_library_manipulator.built_type().reference
  };

  m_types_accessor = std::make_unique<decl_namespace_types_accessor>(accessor);
}

void builtin_decl_namespace_context::add_functions()
{
}

sema::type_builder builtin_decl_namespace_context::add_forwarding_lists_type()
{
  static const auto token = m_builtin_tokens.decl().forwarding_lists_name();
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ token } };
  sema::type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                              name_representation };

  const auto members = {
    builtin_variable_info{
      m_builtin_tokens.decl().forwarding_lists_interface(),
      m_generics_creation_utils.list_of_strings() },
    builtin_variable_info{ m_builtin_tokens.decl().forwarding_lists_public(),
                           m_generics_creation_utils.list_of_strings() },
    builtin_variable_info{ m_builtin_tokens.decl().forwarding_lists_private(),
                           m_generics_creation_utils.list_of_strings() }
  };

  for (const auto& member : members) {
    const auto id_index = sema::identifiers_index_provider::get_next();
    sema::member_info info{ member.name, member.type, id_index };
    builder.with_member(info);
  }

  builder.build_builtin_and_register_in_context();
  return builder;
}

sema::type_builder builtin_decl_namespace_context::add_product_type(
  const sema::sema_type& product_type_type,
  const sema::sema_type& forwarding_lists_type)
{
  static const auto token = m_builtin_tokens.decl().product_name();
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ token } };
  sema::type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                              name_representation };

  const auto members = {
    builtin_variable_info{ m_builtin_tokens.decl().product_type_member(),
                           product_type_type },
    builtin_variable_info{ m_builtin_tokens.decl().product_name_member(),
                           m_builtin_types.string },
    builtin_variable_info{ m_builtin_tokens.decl().product_name_prefix(),
                           m_builtin_types.string },
    builtin_variable_info{ m_builtin_tokens.decl().product_name_suffix(),
                           m_builtin_types.string },
    builtin_variable_info{ m_builtin_tokens.decl().product_files(),
                           forwarding_lists_type },
    builtin_variable_info{ m_builtin_tokens.decl().product_include_dirs(),
                           forwarding_lists_type },
    builtin_variable_info{ m_builtin_tokens.decl().product_compile_options(),
                           forwarding_lists_type },
    builtin_variable_info{ m_builtin_tokens.decl().product_dependencies(),
                           forwarding_lists_type }
  };

  for (const auto& member : members) {
    const auto id_index = sema::identifiers_index_provider::get_next();
    sema::member_info info{ member.name, member.type, id_index };
    builder.with_member(info);
  }

  builder.build_builtin_and_register_in_context();
  return builder;
}

const sema::sema_type& builtin_decl_namespace_context::add_product_type_type()
{
  const std::vector<lexer::token> enumerators{
    m_builtin_tokens.decl().product_type_executable(),
    m_builtin_tokens.decl().product_type_static_library(),
    m_builtin_tokens.decl().product_type_shared_library()
  };

  return add_enum_type(m_builtin_tokens.decl().product_type_name(),
                       enumerators, m_builtin_types);
}

sema::type_builder builtin_decl_namespace_context::add_shared_library_type(
  const sema::sema_type& product_type)
{
  static const auto token = m_builtin_tokens.decl().shared_library_name();
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ token } };
  sema::type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                              name_representation };

  builder.with_derived_type(product_type);

  builder.build_builtin_and_register_in_context();
  return builder;
}

sema::type_builder builtin_decl_namespace_context::add_static_library_type(
  const sema::sema_type& product_type)
{
  static const auto token = m_builtin_tokens.decl().static_library_name();
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ token } };
  sema::type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                              name_representation };

  builder.with_derived_type(product_type);

  builder.build_builtin_and_register_in_context();
  return builder;
}

sema::type_builder builtin_decl_namespace_context::add_executable_type(
  const sema::sema_type& product_type)
{
  static const auto token = m_builtin_tokens.decl().executable_name();
  const auto name_representation =
    ast::type_representation{ ast::qualified_name{ token } };
  sema::type_builder builder{ m_factories, m_qualified_ctxs.types, *this,
                              name_representation };

  builder.with_derived_type(product_type);

  builder.build_builtin_and_register_in_context();
  return builder;
}

const decl_namespace_types_accessor&
builtin_decl_namespace_context::types_accessor() const
{
  return *m_types_accessor;
}
}
