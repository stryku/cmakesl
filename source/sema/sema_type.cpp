#include "sema/sema_type.hpp"
#include "sema/sema_context.hpp"
#include "sema_type.hpp"

#include <algorithm>

namespace cmsl::sema {

sema_type::sema_type(const sema_type_reference& reference, flags_t f)
  : m_ctx{ reference.referenced_type.m_ctx }
  , m_name{ reference.referenced_type.name().name(),
            ast::type_representation::is_reference_tag{} }
  , m_members{ reference.referenced_type.m_members }
  , m_referenced_type{ &reference.referenced_type }
  , m_flags{ f }
{
}

sema_type::sema_type(const sema_context& ctx, ast::type_representation name,
                     std::vector<member_info> members, flags_t f,
                     const sema_type* derived_type)
  : m_ctx{ ctx }
  , m_name{ std::move(name) }
  , m_members{ std::move(members) }
  , m_flags{ f }
  , m_derived_type{ derived_type }
{
}

const ast::type_representation& sema_type::name() const
{
  return m_name;
}

const sema_context& sema_type::context() const
{
  return m_ctx;
}

std::optional<member_info> sema_type::find_member(cmsl::string_view name) const
{
  const auto pred = [name](const auto& member) {
    return member.name.str() == name;
  };

  auto found =
    std::find_if(std::cbegin(m_members), std::cend(m_members), pred);
  if (found != std::cend(m_members)) {
    return *found;
  }

  return m_derived_type ? m_derived_type->find_member(name) : std::nullopt;
}

single_scope_function_lookup_result_t sema_type::find_member_function(
  token_t name) const
{
  auto result = m_ctx.find_function_in_this_scope(name);
  if (m_derived_type) {
    const auto derived_result = m_derived_type->find_member_function(name);
    std::copy(std::cbegin(derived_result), std::cend(derived_result),
              std::back_inserter(result));
  }

  return result;
}

bool sema_type::operator==(const sema_type& rhs) const
{

  return this == &rhs || name() == rhs.name();
}

bool sema_type::operator!=(const sema_type& rhs) const
{
  return !(*this == rhs);
}

bool sema_type::is_complex() const
{
  return !m_members.empty() ||
    (m_derived_type && m_derived_type->is_complex());
}

const std::vector<member_info>& sema_type::members() const
{
  return m_members;
}

bool sema_type::is_reference() const
{
  return m_referenced_type != nullptr;
}

const sema_type& sema_type::referenced_type() const
{
  return *m_referenced_type;
}

bool sema_type::is_designated_initializer() const
{
  return m_flags & flags::designated_initializer;
}

bool sema_type::is_enum() const
{
  return m_flags & flags::enum_;
}

bool sema_type::is_builtin() const
{
  return m_flags & flags::builtin;
}

std::string sema_type::fully_qualified_name() const
{
  auto name = m_ctx.fully_qualified_name();
  if (is_reference()) {
    name += '&';
  }

  return name;
}
const sema_type* sema_type::derived_type() const
{
  return m_derived_type;
}

bool sema_type::derives_from(const sema_type& base) const
{
  const auto derived = decayed().derived_type();
  if (!derived) {
    return false;
  }

  return *derived == base.decayed() || m_derived_type->derives_from(base);
}

const sema_type& sema_type::decayed() const
{
  return is_reference() ? referenced_type() : *this;
}
}
