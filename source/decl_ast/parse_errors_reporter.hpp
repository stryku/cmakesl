#pragma once

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"
#include "parse/parse_errors_observer.hpp"

namespace cmsl::decl_ast {
class parse_errors_observer : public parse::parse_errors_observer
{
public:
  virtual ~parse_errors_observer() = default;

  virtual void raise_expected_component_or_property(lexer::token token) = 0;
  virtual void raise_unexpected_list_in_list(lexer::token token) = 0;
};

class parse_errors_reporter : public parse_errors_observer
{
public:
  explicit parse_errors_reporter(errors::errors_observer& errs_observer,
                                 cmsl::source_view source);

  void raise_unexpected_end_of_file(lexer::token token) override;
  void raise_expected_token(lexer::token token,
                            lexer::token_type type) override;
  void raise_unexpected_token(lexer::token token) override;
  void raise_expected_keyword(lexer::token token,
                              lexer::token_type keyword) override;
  void raise_expected_expression(lexer::token token) override;
  void raise_expected_component_or_property(lexer::token token) override;
  void raise_unexpected_list_in_list(lexer::token token) override;

private:
  void raise(lexer::token token, std::string message);

private:
  errors::errors_observer& m_errors_observer;
  cmsl::source_view m_source;
};
}
