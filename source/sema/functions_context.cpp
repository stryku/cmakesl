#include "sema/functions_context.hpp"
#include "sema/function_signature.hpp"
#include "sema/qualified_contexts_dumper.hpp"
#include "sema/sema_function.hpp"

namespace cmsl::sema {

void functions_context_impl::register_function(const lexer::token& name,
                                               const sema_function& fun,
                                               bool exported)
{
  m_functions_finder.register_entry(name, function_data{ fun }, exported);
}

void functions_context_impl::enter_global_ctx(const lexer::token& name,
                                              bool exported)
{
  m_functions_finder.enter_global_node(name, exported);
}

void functions_context_impl::leave_ctx()
{
  m_functions_finder.leave_node();
}

function_lookup_result_t functions_context_impl::find(
  const std::vector<ast::name_with_coloncolon>& names) const
{
  const auto found_entries = m_functions_finder.find(names);
  if (found_entries.empty()) {
    return {};
  }

  single_scope_function_lookup_result_t results;

  std::transform(std::cbegin(found_entries), std::cend(found_entries),
                 std::back_inserter(results),
                 [](const auto& entry_info) { return &entry_info.entry.fun; });

  return function_lookup_result_t{ std::move(results) };
}

const sema_function* functions_context_impl::find_in_current_scope(
  const function_signature& signature) const
{
  const auto found_functions =
    m_functions_finder.find_in_current_node(signature.name);

  const auto found_exact_function =
    std::find_if(std::cbegin(found_functions), std::cend(found_functions),
                 [&signature](const auto& entry_info) {
                   return entry_info.entry.fun.signature() == signature;
                 });

  if (found_exact_function == std::cend(found_functions)) {
    return nullptr;
  }

  return &found_exact_function->entry.fun;
}

std::unique_ptr<functions_context> functions_context_impl::clone() const
{
  auto created = std::make_unique<functions_context_impl>();
  created->m_functions_finder = m_functions_finder;
  return std::move(created);
}

std::unique_ptr<functions_context>
functions_context_impl::collect_exported_stuff() const
{
  auto created = std::make_unique<functions_context_impl>();
  created->m_functions_finder = m_functions_finder.collect_exported_stuff();
  return std::move(created);
}

bool functions_context_impl::merge_imported_stuff(
  const functions_context& imported, errors::errors_observer& errs)
{
  const auto& casted = static_cast<const functions_context_impl&>(imported);
  return m_functions_finder.merge_imported_stuff(casted.m_functions_finder,
                                                 errs);
}

void functions_context_impl::dump(qualified_contexts_dumper& dumper) const
{
  dumper.dump(m_functions_finder);
}
}
