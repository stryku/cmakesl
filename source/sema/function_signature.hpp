#pragma once

#include "lexer/token.hpp"
#include <vector>

namespace cmsl::sema {
class sema_type;

struct parameter_declaration
{
  const sema_type& ty;
  lexer::token name;
};

struct function_signature
{
  lexer::token name;
  std::vector<parameter_declaration> params{};
};
}
