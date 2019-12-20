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
  //  , m_builtin_types{ builtin_types }
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
  add_forwarding_lists_type();
}

void builtin_decl_namespace_context::add_functions()
{
}

const sema::sema_type&
builtin_decl_namespace_context::add_forwarding_lists_type()
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
  return builder.built_type().ty;
}

}
