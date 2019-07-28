#include "ast/qualified_name.hpp"
#include "qualified_name.hpp"

namespace cmsl::ast {

ast::qualified_name::qualified_name(lexer::token name)
{
  m_names.emplace_back(name_with_coloncolon{ name });
}

ast::qualified_name::qualified_name(std::vector<name_with_coloncolon> names)
  : m_names{ std::move(names) }
{
}

std::string qualified_name::to_string() const
{
  std::string result;

  for (const auto& name : m_names) {
    result += name.name.str();
    if (name.coloncolon) {
      result += "::";
    }
  }

  return result;
}

bool ast::qualified_name::is_auto() const
{
  return m_names.size() == 1u &&
    m_names[0].name.get_type() == lexer::token_type::kw_auto;
}

source_range qualified_name::src_range() const
{
  const auto begin = first_name_token().src_range().begin;
  const auto end = m_names.back().name.src_range().end;
  return source_range{ begin, end };
}

source_view qualified_name::source() const
{
  return first_name_token().source();
}

const lexer::token& ast::qualified_name::first_name_token() const
{
  const auto starts_with_global_access = m_names[0].name.str().empty();
  if (!starts_with_global_access) {
    return m_names[0].name;
  } else {
    return m_names[1].name;
  }
}

const std::vector<name_with_coloncolon>& qualified_name::names() const
{
  return m_names;
}

std::vector<lexer::token> qualified_name::tokens() const
{
  std::vector<lexer::token> result;

  for (const auto& name : m_names) {
    result.emplace_back(name.name);
    if (name.coloncolon) {
      result.emplace_back(*name.coloncolon);
    }
  }

  return result;
}

bool qualified_name::is_fully_qualified() const
{
  return m_names.front().name.str() == "";
}
}
