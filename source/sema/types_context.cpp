#include "sema/types_context.hpp"
#include "sema/sema_context.hpp"
#include "sema/sema_type.hpp"

namespace cmsl::sema {

void types_context_impl::register_type(const lexer::token& name,
                                       const sema_type& ty)
{
  m_types_finder.register_entry(name, type_data{ ty });
}

const sema_type* types_context_impl::find(
  const std::vector<ast::name_with_coloncolon>& names) const
{
  const auto found_entries = m_types_finder.find(names);
  if (found_entries.empty()) {
    return nullptr;
  }

  // At this point the only entries in the vector should be the type and its
  // reference. Return the type that is not the reference.
  if (!found_entries[0].entry.ty.is_reference()) {
    return &found_entries[0].entry.ty;
  } else {
    return &found_entries[1].entry.ty;
  }
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

void types_context_impl::enter_global_ctx(const lexer::token& name)
{
  m_types_finder.enter_global_node(name);
}

void types_context_impl::leave_ctx()
{
  m_types_finder.leave_node();
}

}
