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
                     std::vector<member_info> members, flags_t f)
  : m_ctx{ ctx }
  , m_name{ std::move(name) }
  , m_members{ std::move(members) }
  , m_flags{ f }
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
  if (found == std::cend(m_members)) {
    return {};
  }

  return *found;
}

single_scope_function_lookup_result_t sema_type::find_member_function(
  token_t name) const
{
  return m_ctx.find_function_in_this_scope(name);
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
  return !m_members.empty();
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
}
