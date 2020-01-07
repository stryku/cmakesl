#include "parse/parser_utils.hpp"

#include "common/algorithm.hpp"
#include "parse/parse_errors_observer.hpp"

namespace cmsl::parse {
parser_utils::parser_utils(parse_errors_observer& err_observer,
                           token_it current, token_it end)
  : m_current{ current }
  , m_end{ end }
  , m_prev{ m_current }
  , m_err_observer{ err_observer }
{
}

bool parser_utils::is_at_end() const
{
  return m_current == m_end;
}

bool parser_utils::expect_not_at_end()
{
  if (is_at_end()) {
    m_err_observer.raise_unexpected_end_of_file(
      get_token_for_is_at_end_error_report());
    return false;
  }

  return true;
}

parser_utils::token_type_t parser_utils::peek(size_t n) const
{
  return peek_from(m_current, n);
}

parser_utils::token_type_t parser_utils::peek_from(token_it it, size_t n) const
{
  if (static_cast<size_t>(std::distance(it, m_end)) <= n) {
    return token_type_t::undef;
  }

  return std::next(it, n)->get_type();
}

std::optional<parser_utils::token_t> parser_utils::eat(
  std::optional<token_type_t> type)
{
  if (!expect_not_at_end()) {
    return {};
  }

  if (type && !current_is(*type)) {
    m_err_observer.raise_expected_token(*m_current, *type);
    return {};
  }

  const auto t = *m_current;
  m_prev = m_current;
  ++m_current;
  return t;
}

const parser_utils::token_t& parser_utils::current() const
{
  return *m_current;
}

const parser_utils::token_t& parser_utils::prev() const
{
  return *m_prev;
}

parser_utils::token_type_t parser_utils::curr_type() const
{
  return is_at_end() ? token_type_t::undef : m_current->get_type();
}

bool parser_utils::next_is(token_type_t token_type) const
{
  return next_to_is(m_current, token_type);
}

bool parser_utils::next_to_is(token_it it, token_type_t token_type) const
{
  return peek_from(it) == token_type;
}

bool parser_utils::current_is(token_type_t token_type) const
{
  if (is_at_end()) {
    return false;
  }

  return curr_type() == token_type;
}

bool parser_utils::current_is_type() const
{
  return lexer::is_builtin_type(curr_type()) ||
    current_is(token_type_t::identifier);
}

bool parser_utils::current_is_name_of_function_call() const
{
  return is_name_of_function_call(curr_type());
}

bool parser_utils::is_name_of_function_call(token_type_t type) const
{
  // Constructor of user defined function.
  return lexer::is_builtin_type(type) || type == token_type_t::identifier;
}

bool parser_utils::type_of_token_is(token_it it, token_type_t token_type) const
{
  return it != m_end && it->get_type() == token_type;
}

parser_utils::token_it parser_utils::current_iterator() const
{
  return m_current;
}

parser_utils::token_it parser_utils::end_iterator() const
{
  return m_end;
}

void parser_utils::adjust_current_iterator(
  parser_utils::token_it new_current_it)
{
  m_current = new_current_it;
}

std::optional<parser_utils::token_t> parser_utils::try_eat(token_type_t type)
{
  if (current_is(type)) {
    return eat(type);
  }

  return {};
}

parser_utils::token_t parser_utils::get_token_for_is_at_end_error_report()
  const
{
  if (m_prev == m_end) {
    return token_t{};
  }

  return *m_prev;
}

parser_utils::token_t parser_utils::get_token_for_error_report() const
{
  if (is_at_end()) {
    return get_token_for_is_at_end_error_report();
  }

  return current();
}

bool parser_utils::current_is_unary_operator() const
{
  return lexer::is_unary_operator(curr_type());
}

bool parser_utils::current_is_identifier_with_value(
  cmsl::string_view value) const
{
  return current_is(token_type_t::identifier) && current().str() == value;
}
}
