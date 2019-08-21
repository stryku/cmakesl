#include "sema/sema_context_impl.hpp"
#include "common/assert.hpp"
#include "sema/sema_function.hpp"

#include <algorithm>

namespace cmsl::sema {
sema_context_impl::sema_context_impl(std::string name,
                                     const sema_context* parent,
                                     context_type context_type)
  : m_parent{ parent }
  , m_context_type{ context_type }
  , m_name{ std::move(name) }
{
}

void sema_context_impl::add_function(const sema_function& function)
{
  m_functions.push_back(&function);
}

void sema_context_impl::add_type(const sema_type& type)
{
  m_types.emplace_back(&type);
}

const sema_type* sema_context_impl::find_type(
  const ast::type_representation& name) const
{
  if (auto found = find_type_in_this_scope(name)) {
    return found;
  }

  return m_parent ? m_parent->find_type(name) : nullptr;
}

const sema_type* sema_context_impl::find_referenced_type(
  const ast::type_representation& name) const
{
  if (!name.is_reference()) {
    return nullptr;
  }

  // Todo: optimize creation of string out.
  const auto pred = [referenced_type_name =
                       name.to_string_without_reference()](const auto& type) {
    return referenced_type_name == type->name().to_string_without_reference();
  };

  if (auto found = find_type_in_this_scope_with_predicate(pred)) {
    return found;
  }

  return m_parent ? m_parent->find_referenced_type(name) : nullptr;
}

const sema_type* sema_context_impl::find_type_in_this_scope(
  const ast::type_representation& name) const
{
  const auto pred = [&name](const auto& type) { return name == type->name(); };

  return find_type_in_this_scope_with_predicate(pred);
}

function_lookup_result_t sema_context_impl::find_function(
  const lexer::token& name) const
{
  function_lookup_result_t result;

  if (m_parent) {
    result = m_parent->find_function(name);
  }

  auto this_scope_lookup = find_function_in_this_scope(name);
  result.insert(std::begin(result), std::move(this_scope_lookup));

  return result;
}

single_scope_function_lookup_result_t
sema_context_impl::find_function_in_this_scope(const lexer::token& name) const
{
  single_scope_function_lookup_result_t result;

  // Collect functions.
  const auto pred = [name](const auto& function) {
    return name.str() == function->signature().name.str();
  };
  std::copy_if(std::cbegin(m_functions), std::cend(m_functions),
               std::back_inserter(result), pred);

  // Collect constructors.
  // Todo: test ctors collecting.
  const auto type_name =
    ast::type_representation{ ast::qualified_name{ name } };
  if (auto ty = find_type_in_this_scope(type_name)) {
    const auto& type_ctx = ty->context();
    const auto constructors = type_ctx.find_function_in_this_scope(name);
    result.insert(std::end(result), std::cbegin(constructors),
                  std::cend(constructors));
  }

  return result;
}

sema_context::context_type sema_context_impl::type() const
{
  return m_context_type;
}

template <typename Predicate>
const sema_type* sema_context_impl::find_type_in_this_scope_with_predicate(
  Predicate&& pred) const
{
  const auto found =
    std::find_if(std::cbegin(m_types), std::cend(m_types), pred);
  return found != std::cend(m_types) ? *found : nullptr;
}

const sema_type* sema_context_impl::find_reference_for(
  const sema_type& type) const
{
  for (const auto& t : m_types) {
    if (t->is_reference() && t->referenced_type() == type) {
      return t;
    }
  }

  return m_parent ? m_parent->find_reference_for(type) : nullptr;
}

std::vector<std::reference_wrapper<const sema_type>> sema_context_impl::types()
  const
{
  std::vector<std::reference_wrapper<const sema_type>> result;
  result.reserve(m_types.size());

  std::transform(std::cbegin(m_types), std::cend(m_types),
                 std::back_inserter(result),
                 [](const auto ty) { return std::ref(*ty); });

  return result;
}

std::string sema_context_impl::fully_qualified_name() const
{
  const auto name = std::string{ m_name };
  if (m_parent) {
    return m_parent->fully_qualified_name() + "::" + name;
  }

  return name;
}

const sema_context* sema_context_impl::find_ctx_containing_type(
  const std::string& name) const
{
  //  if (m_name == name) {
  //    return this;
  //  }

  const auto fully_qualified_type_name = fully_qualified_name() + "::" + name;
  for (const auto ty : m_types) {
    if (ty->fully_qualified_name() == fully_qualified_type_name) {
      return this;
    }
  }

  if (m_parent) {
    return m_parent->find_ctx_containing_type(name);
  }

  return nullptr;
}

const sema_context* sema_context_impl::find_ctx_for_type_lookup(
  const ast::type_representation& name) const
{
  if (!name.is_generic()) {
    const auto& qual_name = name.qual_name();

    if (qual_name.is_fully_qualified()) {
      return find_root();
    }

    return find_ctx_containing_type(name.to_string());
  } else {
    CMSL_UNREACHABLE("Trying to find context for a generic type");
    return nullptr;
  }
}

const sema_context* sema_context_impl::find_root() const
{
  const sema_context* ctx = this;
  while (ctx->parent()) {
    ctx = ctx->parent();
  }

  return ctx;
}
}
