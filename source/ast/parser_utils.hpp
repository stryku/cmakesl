#pragma once

#include "ast/qualified_name.hpp"
#include "lexer/token.hpp"
#include "parse/parser_utils.hpp"

#include <optional>

namespace cmsl::ast {
class parse_errors_observer;

class parser_utils : public parse::parser_utils
{
protected:
  explicit parser_utils(parse_errors_observer& err_observer, token_it current,
                        token_it end);

  bool possibly_qualified_name_starts() const;

  std::optional<std::vector<ast::name_with_coloncolon>>
  parse_possibly_qualified_name();
};
}