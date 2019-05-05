#pragma once

#include "ast/type_representation.hpp"

#include <optional>

namespace cmsl::ast {
struct param_declaration
{
  type_representation ty;
  lexer::token name;
};
}
