#include "decl_ast/parse_errors_reporter.hpp"

namespace cmsl::decl_ast {
parse_errors_reporter::parse_errors_reporter(
  errors::errors_observer& errs_observer, cmsl::source_view source)
  : m_errors_observer{ errs_observer }
  , m_source{ source }
{
}

void parse_errors_reporter::raise_unexpected_end_of_file(lexer::token token)
{
  raise(token, "unexpected end of file");
}

void parse_errors_reporter::raise_expected_token(lexer::token token,
                                                 lexer::token_type type)
{
  raise(token, "expected " + to_string(type));
}

void parse_errors_reporter::raise_unexpected_token(lexer::token token)
{
  raise(token, "unexpected token");
}

void parse_errors_reporter::raise_expected_keyword(lexer::token token,
                                                   lexer::token_type keyword)
{
  raise(token, "expected " + to_string(keyword));
}

void parse_errors_reporter::raise_expected_expression(lexer::token token)
{
  raise(token, "expected expression");
}

void parse_errors_reporter::raise_expected_component_or_property(
  lexer::token token)
{
  raise(token, "expected component or property");
}

void parse_errors_reporter::raise_unexpected_list_in_list(lexer::token token)
{
  raise(token, "list can not contain another list");
}

void parse_errors_reporter::raise(lexer::token token, std::string message)
{
  const auto line_info = m_source.line(token.src_range().begin.line);
  auto err = errors::error{ m_source.path(),           line_info.line,
                            line_info.start_pos,       std::move(message),
                            errors::error_type::error, token.src_range() };
  m_errors_observer.notify_error(std::move(err));
}

void parse_errors_reporter::raise_expected_as_type(lexer::token token)
{
  raise(token,
        "expected 'as_bool', 'as_int', 'as_double', 'as_string' or 'as_list'");
}
}
