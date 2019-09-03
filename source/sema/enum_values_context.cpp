#include "sema/enum_values_context.hpp"

namespace cmsl::sema {

void enum_values_context_impl::register_identifier(
  lexer::token declaration_token, enum_value_info info)
{
  m_finder.register_entry(declaration_token, info);
}

std::optional<enum_values_context::enum_value_info>
enum_values_context_impl::info_of(
  const enum_values_context::qualified_names_t& names) const
{
  const auto found_entries = m_finder.find(names);
  if (found_entries.empty()) {
    return std::nullopt;
  }

  // At this point we know that there is only one enum value with given name.
  return found_entries[0].entry;
}

void enum_values_context_impl::enter_global_ctx(
  enum_values_context::token_t name)
{
  m_finder.enter_global_node(name);
}

void enum_values_context_impl::leave_ctx()
{
  m_finder.leave_node();
}

std::unique_ptr<enum_values_context> enum_values_context_impl::clone() const
{
  auto created = std::make_unique<enum_values_context_impl>();
  created->m_finder = m_finder;
  return std::move(created);
}
}
