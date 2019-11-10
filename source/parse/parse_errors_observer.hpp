#pragma once

#include "lexer/token.hpp"

namespace cmsl::parse {
class parse_errors_observer
{
public:
  virtual ~parse_errors_observer() = default;

  virtual void raise_unexpected_end_of_file(lexer::token token) = 0;
  virtual void raise_expected_token(lexer::token token,
                                    lexer::token_type type) = 0;
  virtual void raise_unexpected_token(lexer::token token) = 0;
  virtual void raise_expected_keyword(lexer::token token,
                                      lexer::token_type keyword) = 0;
  virtual void raise_expected_expression(lexer::token token) = 0;
};
}
