#pragma once

#include "lexer/token.hpp"

#include <vector>

namespace cmsl::sema {
class sema_type;

struct parameter_declaration
{
  const sema_type& ty;
  lexer::token name;
  unsigned index;

  bool operator==(const parameter_declaration& rhs) const;
};

struct function_signature
{
  lexer::token name;
  std::vector<parameter_declaration> params{};

  bool operator==(const function_signature& rhs) const;
};
}
