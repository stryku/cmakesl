#pragma once

#include "ast/qualified_name.hpp"
#include "lexer/token.hpp"

#include <optional>

namespace cmsl {
namespace ast {
class parse_errors_observer;

class parser_utils
{
protected:
  using token_container_t = lexer::token_container_t;
  using token_it = token_container_t::const_iterator;
  using token_type_t = lexer::token_type;
  using token_t = lexer::token;

  explicit parser_utils(parse_errors_observer& err_observer, token_it current,
                        token_it end);

  bool expect_not_at_end();
  bool is_at_end() const;

  token_type_t peek(size_t n = 1u) const;
  token_type_t peek_from(token_it it, size_t n = 1u) const;
  std::optional<token_t> eat(std::optional<token_type_t> type = {});
  std::optional<token_t> try_eat(token_type_t type);

  const token_t& current() const;
  const token_t& prev() const;
  token_type_t curr_type() const;
  bool next_is(token_type_t token_type) const;
  bool next_to_is(token_it it, token_type_t token_type) const;
  bool current_is(token_type_t token_type) const;
  bool current_is_type() const;
  bool current_is_name_of_function_call() const;
  bool type_of_token_is(token_it it, token_type_t token_type) const;
  bool is_builtin_type(token_type_t token_type) const;
  bool possibly_qualified_name_starts() const;

  void adjust_current_iterator(token_it new_current_it);
  token_it current_iterator() const;
  token_it end_iterator() const;

  token_t get_token_for_error_report() const;
  token_t get_token_for_is_at_end_error_report() const;

  std::optional<std::vector<ast::name_with_coloncolon>>
  parse_possibly_qualified_name();

private:
  parse_errors_observer& m_err_observer;
  token_it m_current;
  token_it m_end;
  token_it m_prev;
};
}
}