#pragma once

#include "lexer/token.hpp"

#include <optional>
#include <ostream>

namespace cmsl::ast {
struct name_with_coloncolon
{
  lexer::token name;
  std::optional<lexer::token>
    coloncolon; // :: operator after the namespace name.

  bool operator==(const name_with_coloncolon& rhs) const
  {
    return std::tie(name, coloncolon) == std::tie(rhs.name, rhs.coloncolon);
  }
};

class qualified_name
{
public:
  qualified_name(lexer::token name);
  qualified_name(std::vector<name_with_coloncolon> names);

  const std::vector<name_with_coloncolon>& names() const;

  std::string to_string() const;

  bool is_auto() const;
  bool is_fully_qualified() const;

  source_range src_range() const;
  source_view source() const;

  std::vector<lexer::token> tokens() const;

private:
  const lexer::token& first_name_token() const;

private:
  std::vector<name_with_coloncolon> m_names;
};

using qualified_name_t = std::vector<name_with_coloncolon>;
}