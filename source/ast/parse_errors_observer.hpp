#pragma once

#include "lexer/token.hpp"

#include "errors/error.hpp"
#include "errors/errors_observer.hpp"

namespace cmsl::ast {
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
  virtual void raise_expected_type(lexer::token token) = 0;
  virtual void raise_expected_parameter_declaration(lexer::token token) = 0;
  virtual void raise_expected_parameter(lexer::token token) = 0;
  virtual void raise_expected_block(lexer::token token) = 0;
  virtual void raise_expected_expression(lexer::token token) = 0;
};

class parse_errors_reporter : public parse_errors_observer
{
public:
  explicit parse_errors_reporter(errors::errors_observer& errs_observer,
                                 cmsl::source_view source)
    : m_errors_observer{ errs_observer }
    , m_source{ source }
  {
  }

  void raise_unexpected_end_of_file(lexer::token token) override
  {
    raise(token, "unexpected end of file");
  }

  void raise_expected_token(lexer::token token,
                            lexer::token_type type) override
  {
    raise(token, "expected " + to_string(type));
  }

  void raise_unexpected_token(lexer::token token) override
  {
    raise(token, "unexpected token");
  }

  void raise_expected_type(lexer::token token) override
  {
    raise(token, "expected a type");
  }

  void raise_expected_keyword(lexer::token token,
                              lexer::token_type keyword) override
  {
    raise(token, "expected " + to_string(keyword));
  }

  void raise_expected_parameter_declaration(lexer::token token) override
  {
    raise(token, "expected a parameter declaration");
  }

  void raise_expected_parameter(lexer::token token) override
  {
    raise(token, "expected a parameter");
  }

  void raise_expected_block(lexer::token token) override
  {
    raise(token, "expected block");
  }

  void raise_expected_expression(lexer::token token) override
  {
    raise(token, "expected expression");
  }

private:
  void raise(lexer::token token, std::string message)
  {
    const auto line_info = m_source.line(token.src_range().begin.line);
    auto err = errors::error{ m_source.path(),           line_info.line,
                              line_info.start_pos,       std::move(message),
                              errors::error_type::error, token.src_range() };
    m_errors_observer.nofify_error(std::move(err));
  }

private:
  errors::errors_observer& m_errors_observer;
  cmsl::source_view m_source;
};

class parse_errors_sink : public parse_errors_observer
{
public:
  void raise_unexpected_end_of_file(lexer::token) override {}
  void raise_expected_token(lexer::token, lexer::token_type) override {}
  void raise_unexpected_token(lexer::token) override {}
  void raise_expected_type(lexer::token) override {}
  void raise_expected_keyword(lexer::token, lexer::token_type) override {}
  void raise_expected_parameter_declaration(lexer::token) override {}
  void raise_expected_parameter(lexer::token) override {}
  void raise_expected_block(lexer::token) override {}
  void raise_expected_expression(lexer::token) override {}
};
}
