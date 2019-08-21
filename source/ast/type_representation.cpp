#include "ast/type_representation.hpp"
#include "common/assert.hpp"
#include "common/overloaded.hpp"

namespace cmsl::ast {
// type_representation::type_representation(
//  qualified_name_t qualified_name,
//  lexer::token primary_name, lexer::token reference_token,
//  std::vector<type_representation> nested_types)
//  : m_qualified_name{std::move(qualified_name)}
//  ,m_tokens{ { primary_name } }
//  , m_reference_token{ reference_token }
//  , m_nested_types{ std::move(nested_types) }
//{
//}
//
// type_representation::type_representation(
//  qualified_name_t qualified_name,
//  lexer::token_container_t tokens, lexer::token reference_token,
//  std::vector<type_representation> nested_types)
//  : m_qualified_name{std::move(qualified_name)}
//  ,m_tokens{ std::move(tokens) }
//  , m_reference_token{ reference_token }
//  , m_nested_types{ std::move(nested_types) }
//{
//}
//
// type_representation::type_representation(
//  qualified_name_t qualified_name,
//  lexer::token primary_name, is_reference_tag,
//  std::vector<type_representation> nested_types)
//  : m_qualified_name{std::move(qualified_name)}
//  ,m_tokens{ { primary_name } }
//  , m_is_reference{ true }
//  , m_nested_types{ std::move(nested_types) }
//{
//}
//
// type_representation::type_representation(
//  qualified_name_t qualified_name,
//  lexer::token_container_t tokens, is_reference_tag,
//  std::vector<type_representation> nested_types)
//  : m_qualified_name{std::move(qualified_name)}
//  ,m_tokens{ std::move(tokens) }
//  , m_is_reference{ true }
//  , m_nested_types{ std::move(nested_types) }
//{
//}
//
// type_representation::type_representation(
//  qualified_name_t qualified_name,
//  lexer::token primary_name, std::vector<type_representation> nested_types)
//  : m_qualified_name{std::move(qualified_name)}
//  ,m_tokens{ { primary_name } }
//  , m_nested_types{ std::move(nested_types) }
//{
//}
//
// type_representation::type_representation(
//  qualified_name_t qualified_name,
//  lexer::token_container_t tokens,
//  std::vector<type_representation> nested_types)
//  :m_qualified_name{std::move(qualified_name)}
//  ,m_tokens{ std::move(tokens) }
//  , m_nested_types{ std::move(nested_types) }
//{
//}

///////

type_representation::type_representation(lexer::token name)
  : type_representation{ qualified_name{ name } }
{
}

type_representation::type_representation(lexer::token name,
                                         is_reference_tag tag)
  : type_representation{ qualified_name{ name }, tag }
{
}

type_representation::type_representation(qualified_name name)
  : m_name{ std::move(name) }
{
}

type_representation::type_representation(qualified_name name, is_reference_tag)
  : m_name{ std::move(name) }
  , m_is_reference{ true }
{
}

type_representation::type_representation(generic_type_name name)
  : m_name{ std::move(name) }
{
}

type_representation::type_representation(generic_type_name name,
                                         is_reference_tag)
  : m_name{ std::move(name) }
  , m_is_reference{ true }
{
}

type_representation::type_representation(type_representation::name_t name)
  : m_name{ std::move(name) }
{
}

type_representation::type_representation(type_representation::name_t name,
                                         type_representation::is_reference_tag)
  : m_name{ std::move(name) }
  , m_is_reference{ true }
{
}

///////

// lexer::token type_representation::primary_name() const
//{
//  return m_tokens.front();
//}

const std::vector<type_representation>& type_representation::nested_types()
  const
{
  if (!is_generic()) {
    CMSL_UNREACHABLE("Getting nested types of a non generic type");
  }

  return std::get<generic_type_name>(m_name).nested_types;
}

// const lexer::token_container_t& type_representation::tokens() const
//{
//  return m_tokens;
//}

// const qualified_name_t& type_representation::qualified_name() const
//{
//  return std::get<qualified_name_t >(m_name);
//}

std::string type_representation::to_string() const
{
  auto str = to_string_without_reference();
  if (is_reference()) {
    str += '&';
  }

  return str;
}

std::string type_representation::to_string_without_reference() const
{
  // Todo: create string once, store it somewhere (some strings container) and
  // return string_view.

  auto visitor = [](const auto& name) { return name.to_string(); };

  return std::visit(visitor, m_name);
}

bool type_representation::operator==(const type_representation& rhs) const
{
  // For now, we need to compare strings because when we execute
  // add_subdirectory, a new builtin context is created there. Because of that,
  // we have two instances representing e.g. int type. That should be fixed at
  // some point.
  return this == &rhs || to_string() == rhs.to_string();
}

bool type_representation::operator!=(const type_representation& rhs) const
{
  return !(*this == rhs);
}

bool type_representation::is_generic() const
{
  return std::holds_alternative<generic_type_name>(m_name);
}

bool type_representation::is_reference() const
{
  return m_reference_token.has_value() || m_is_reference;
}

bool type_representation::is_auto() const
{
  if (is_generic()) {
    return false;
  }

  return std::get<qualified_name>(m_name).is_auto();
}

const type_representation::name_t& type_representation::name() const
{
  return m_name;
}

const type_representation::generic_type_name&
type_representation::generic_name() const
{
  return std::get<generic_type_name>(m_name);
}

source_range type_representation::src_range() const
{
  auto visitor =
    overloaded{ [](const generic_type_name& name) {
                 return source_range{ name.tokens.front().src_range().begin,
                                      name.tokens.back().src_range().end };
               },
                [](const qualified_name& name) { return name.src_range(); } };

  return std::visit(visitor, m_name);
}

source_view type_representation::source() const
{
  auto visitor =
    overloaded{ [](const generic_type_name& name) {
                 return name.tokens.front().source();
               },
                [](const qualified_name& name) { return name.source(); } };

  return std::visit(visitor, m_name);
}

lexer::token_container_t type_representation::tokens() const
{
  auto visitor =
    overloaded{ [](const generic_type_name& name) { return name.tokens; },
                [](const qualified_name& name) { return name.tokens(); } };

  return std::visit(visitor, m_name);
}

const qualified_name& type_representation::qual_name() const
{
  return std::get<qualified_name>(m_name);
}

std::string type_representation::unqualified_name() const
{
  auto visitor =
    overloaded{ [](const generic_type_name& name) { return name.to_string(); },
                [](const qualified_name& name) {
                  return std::string{ name.names().back().name.str() };
                } };

  return std::visit(visitor, m_name);
}

lexer::token type_representation::primary_name_token() const
{
  auto visitor = overloaded{
    [](const generic_type_name& name) { return name.primary_name(); },
    [](const qualified_name& name) { return name.names().back().name; }
  };

  return std::visit(visitor, m_name);
}
}

std::ostream& operator<<(std::ostream& out,
                         const cmsl::ast::type_representation& type_repr)
{
  return out << type_repr.to_string();
}
