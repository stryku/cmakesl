#include "sema/identifiers_context.hpp"
#include "ast/qualified_name.hpp"

namespace cmsl::sema {

void identifiers_context_impl::register_identifier(
  identifiers_context_impl::token_t declaration_token, identifier_info info)
{
  m_contextes_handler.register_entry(declaration_token, info);
}

std::optional<identifiers_context_impl::identifier_info>
identifiers_context_impl::info_of(const qualified_names_t& names) const
{
  const auto found = m_contextes_handler.find(names);
  if (!found) {
    return std::nullopt;
  }

  return found->entry;
}

std::optional<identifiers_context_impl::token_t>
identifiers_context_impl::declaration_token_of(
  const qualified_names_t& names) const
{
  const auto found = m_contextes_handler.find(names);
  if (!found) {
    return {};
  }

  return found->registration_token;
}

void identifiers_context_impl::enter_local_ctx()
{
  m_contextes_handler.enter_local_node();
}

void identifiers_context_impl::enter_global_ctx(token_t name)
{
  m_contextes_handler.enter_global_node(name);
}

void identifiers_context_impl::leave_ctx()
{
  m_contextes_handler.leave_node();
}

std::optional<identifiers_context_impl::token_t>
identifiers_context_impl::declaration_token_of_ctx(
  const identifiers_context::qualified_names_t& names) const
{
  return m_contextes_handler.find_node_registration_token(names);
}
}
