#include "sema/identifiers_context.hpp"
#include "ast/qualified_name.hpp"

#include <algorithm>

namespace cmsl::sema {

void identifiers_context_impl::register_identifier(
  identifiers_context_impl::token_t declaration_token, identifier_info info,
  bool exported)
{
  m_contextes_handler.register_entry(declaration_token, info, exported);
}

std::optional<identifier_info> identifiers_context_impl::info_of(
  const qualified_names_t& names) const
{
  const auto found_entries = m_contextes_handler.find(names);
  if (found_entries.empty()) {
    return std::nullopt;
  }

  // Sema prevents from defining multiple identifiers with the same name in one
  // scope, so we know that at this point there is only one entry in the
  // vector.
  return found_entries[0].entry;
}

std::optional<identifiers_context_impl::token_t>
identifiers_context_impl::declaration_token_of(
  const qualified_names_t& names) const
{
  const auto found_entries = m_contextes_handler.find(names);
  if (found_entries.empty()) {
    return std::nullopt;
  }

  // Sema prevents from defining multiple identifiers with the same name in one
  // scope, so we know that at this point there is only one entry in the
  // vector.
  return found_entries[0].registration_token;
}

void identifiers_context_impl::enter_local_ctx()
{
  m_contextes_handler.enter_local_node();
}

void identifiers_context_impl::enter_global_ctx(token_t name, bool exported)
{
  m_contextes_handler.enter_global_node(name, exported);
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

std::unique_ptr<identifiers_context> identifiers_context_impl::clone() const
{
  auto created = std::make_unique<identifiers_context_impl>();
  created->m_contextes_handler = m_contextes_handler;
  return std::move(created);
}

bool identifiers_context_impl::is_in_global_ctx() const
{
  return m_contextes_handler.is_in_global_context();
}

std::unique_ptr<identifiers_context>
identifiers_context_impl::collect_exported_stuff() const
{
  auto created = std::make_unique<identifiers_context_impl>();
  created->m_contextes_handler = m_contextes_handler.collect_exported_stuff();
  return std::move(created);
}

bool identifiers_context_impl::merge_imported_stuff(
  const identifiers_context& imported, errors::errors_observer& errs)
{
  const auto& casted = static_cast<const identifiers_context_impl&>(imported);
  return m_contextes_handler.merge_imported_stuff(casted.m_contextes_handler,
                                                  errs);
}
}
