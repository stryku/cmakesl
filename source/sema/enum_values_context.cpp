#include "sema/enum_values_context.hpp"
#include "sema/qualified_contexts_dumper.hpp"

namespace cmsl::sema {

void enum_values_context_impl::register_identifier(
  lexer::token declaration_token, enum_value_info info, bool exported)
{
  m_finder.register_entry(declaration_token, info, exported);
}

std::optional<enum_value_info> enum_values_context_impl::info_of(
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
  enum_values_context::token_t name, bool exported)
{
  m_finder.enter_global_node(name, exported);
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

std::unique_ptr<enum_values_context>
enum_values_context_impl::collect_exported_stuff() const
{
  auto created = std::make_unique<enum_values_context_impl>();
  created->m_finder = m_finder.collect_exported_stuff();
  return std::move(created);
}

bool enum_values_context_impl::merge_imported_stuff(
  const enum_values_context& imported, errors::errors_observer& errs)
{
  const auto& casted = static_cast<const enum_values_context_impl&>(imported);
  return m_finder.merge_imported_stuff(casted.m_finder, errs);
}

void enum_values_context_impl::dump(qualified_contexts_dumper& dumper) const
{
  dumper.dump(m_finder);
}
}
