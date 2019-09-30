#include "sema/types_context.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_type.hpp"

namespace cmsl::sema {

void types_context_impl::register_type(const lexer::token& name,
                                       const sema_type& ty, bool exported)
{
  m_types_finder.register_entry(name, type_data{ ty }, exported);
}

std::optional<types_context::type_with_reference> types_context_impl::find(
  const std::vector<ast::name_with_coloncolon>& names) const
{
  const auto found_entries = m_types_finder.find(names);
  if (found_entries.empty()) {
    return std::nullopt;
  }

  // At this point the only entries in the vector should be the type and its
  // reference. Return the type that is not the reference.
  if (!found_entries[0].entry.ty.is_reference()) {
    return type_with_reference{ .ty = found_entries[0].entry.ty,
                                .ref = found_entries[1].entry.ty };
  } else {
    return type_with_reference{ .ty = found_entries[1].entry.ty,
                                .ref = found_entries[0].entry.ty };
  }
}

const sema_type* types_context_impl::find_generic(
  const ast::type_representation& name) const
{
  const auto found_entries =
    m_types_finder.find_in_current_node(name.primary_name_token());
  if (found_entries.empty()) {
    return nullptr;
  }

  for (const auto& entry : found_entries) {
    if (entry.entry.ty.name() == name) {
      return &entry.entry.ty;
    }
  }

  return nullptr;
}

const sema_type* types_context_impl::find_in_current_scope(
  const lexer::token& name) const
{
  const auto found_entries = m_types_finder.find_in_current_node(name);
  if (found_entries.empty()) {
    return nullptr;
  }

  if (!found_entries[0].entry.ty.is_reference()) {
    return &found_entries[0].entry.ty;
  } else {
    return &found_entries[1].entry.ty;
  }
}

void types_context_impl::enter_global_ctx(const lexer::token& name,
                                          bool exported)
{
  m_types_finder.enter_global_node(name, exported);
}

void types_context_impl::leave_ctx()
{
  m_types_finder.leave_node();
}

std::unique_ptr<types_context> types_context_impl::clone() const
{
  auto created = std::make_unique<types_context_impl>();
  created->m_types_finder = m_types_finder;
  return std::move(created);
}

std::unique_ptr<types_context> types_context_impl::collect_exported_stuff()
  const
{
  auto created = std::make_unique<types_context_impl>();
  created->m_types_finder = m_types_finder.collect_exported_stuff();
  return std::move(created);
}

bool types_context_impl::merge_imported_stuff(const types_context& imported,
                                              errors::errors_observer& errs)
{
  const auto& casted = static_cast<const types_context_impl&>(imported);
  return m_types_finder.merge_imported_stuff(casted.m_types_finder, errs);
}
}
