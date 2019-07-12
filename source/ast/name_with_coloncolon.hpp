#pragma once

#include "lexer/token.hpp"

#include <optional>

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
}
